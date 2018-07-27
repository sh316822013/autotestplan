var Timer_elapsedS=0;var Timer_elapsedM=0;var Timer_elapsedH=0;var Timer_totalTimeS=0;var Timer_timerId;var Timer_timerToggle=false;var Timer_isActiveTimer=false;var Timer_intervalDict={};var Timer_clientBase=new pi.Rest.ClientBase();var Timer_taskTimerModal;var Timer_RoundToValue=1;var Timer_RoundToDirection=1;window.addEventListener("message",function(event){if(event.data=="AddNewTaskTimer"){Timer_RestGetTimers();Timer_AddNewTimerClose(false);}if(event.data=="ResizeParentAddNewTimer"){$('#Timer_AddTimeModalForm_IFrame').width('900');}});function Timer_InitTimer(roundToValue,roundToDirection){Timer_RoundToValue=roundToValue;Timer_RoundToDirection=roundToDirection;if($('#Timer_TimerModal').length<=0){Timer_taskTimerModal=pi.UI.CreateDraggableLayer('Timer_TimerModal',false,null,true,'Timer_Close();');Timer_taskTimerModal.SetTitle("Timers");Timer_taskTimerModal.SetBody('<div style="width:400px;max-width:100%" class="AC"><img src="'+Root+'/i/processing.gif"></div>');$("[data-pi-draggable-body='Timer_TimerModal']")[0].id='TaskTimerModal_body';if($('#AddTimerToTask').length<=0){$('#Timer_TimerModal_Options').append('<a id="AddTimerToTask" class="VVLG FL mL15" href="javascript:;" onclick="Timer_AddNewTimer()">Add Timer</a></td><td class="pR15 pB10"><a class="FR Button" href="javascript:;" onclick="Timer_Close()">OK</a>');}$('#Timer_TimerModal').addClass("ViewTable");UITools.ShowAtCenterScreen('Timer_TimerModal',true);Timer_RestGetTimers();}else{Timer_Close();}};function Timer_StartTimer(itemId){Timer_RestCreateTimerForItem(itemId);};function Timer_RenderAddTimeForm(timerId,name,projectOrCompanyName,timeEntryId){var addtimeUrl=Root+"/timer/timer-add-time?";if(timeEntryId!=null&&timeEntryId!=""){addtimeUrl+="timerTimeEntryId="+timeEntryId;}else{addtimeUrl+="timer="+timerId;}var submitTimerModal=pi.UI.CreateDraggableLayer('Timer_AddTimeModalForm',false,null,true,'Timer_CloseAddTimeForm();',true);submitTimerModal.SetTitle(unescape(projectOrCompanyName)+" - "+unescape(name));$('#Timer_AddTimeModalForm').css("background-color","#ffffff");submitTimerModal.SetBody('<div style="width:900px;max-width:90%;box-sizing:border-box;"><div id="Timer_AddTimeModalForm_IFrame" width="900px;" data-iframe-src="'+addtimeUrl+'" data-iframe-height="600"></div></div>');$("#Timer_AddTimeModalForm_Options").append('<div class="FR"><a href="javascript:;" class="VVLG mL15 mB10 Button" onclick="Timer_CloseAddTimeForm();">OK</a></div>');UITools.LoadMetafuseIFrame("Timer_AddTimeModalForm_IFrame");UITools.ShowAtCenterScreen('Timer_AddTimeModalForm',true);Timer_Close();};function Timer_CloseAddTimeForm(){Timer_RestGetTimers(true);UITools.Hide("Timer_AddTimeModalForm");};function Timer_RenderChangeTimerModal(timerId,iconUrl,url,name,roundToValue,roundToDirection){$('#Timer_'+timerId)[0].setAttribute('active',"false");Timer_TimerStop(timerId);var changeTimerModal=pi.UI.CreateDraggableLayer('Timer_ChangeTimerModal',false,null,true,'Timer_ChangeTimerClose(true)');changeTimerModal.SetTitle("Edit Time");var currentTime=$('#Timer_'+timerId).text();currentTime=MathTools.FormatHoursAndMinutesString(currentTime,roundToValue,roundToDirection);var roundJS="";if(roundToValue&&roundToDirection){roundJS=","+roundToValue+","+roundToDirection}changeTimerModal.SetBody('<div style="width:400px;max-width:95%"><div class="pi-input"><a style="background:transparent url('+iconUrl+') scroll no-repeat left center; background-size: 20px 20px; padding:2px 0px 2px 24px; font-size:18px;" onmousedown="hl(this);" href="'+Root+url+'">'+unescape(name)+'</a></div>'+'<div class="pi-input-label AR"><span style="color:#d3d3d3; font-style:italic; font-size:11px;">Enter as HH:MM or decimal time i.e. 1.25 hours</span></div>'+'<div class="pi-input W100 AR"><input id="Timer_changeTimepicker" class="AR" value="'+currentTime+'" onchange="FormTools.EnsureHoursMinutes(this'+roundJS+')" style="width: 225px;" /></div>'+'<div class="FR mT15"><a id="TimerChangeDone" href="javascript:;" class="VVLG Button" onclick="Timer_ChangeTimerClose(false, \''+timerId+'\');">OK</a>'+'<a id="TimerChangeCancel" href="javascript:;" onclick="Timer_ChangeTimerClose(false, \''+timerId+'\');">Cancel</a></div></div>');$('#Timer_ChangeTimerModal').css("background-color","#ffffff");UITools.ShowAtCenterScreen('Timer_ChangeTimerModal',true);};function Timer_RenderErrorModal(){if($('#Timer_ErrorModal').length>0){$('#Timer_ErrorModal').remove();}var deletedModal=pi.UI.CreateDraggableLayer('Timer_ErrorModal',false,400,true,'Timer_Close();Timer_TimerDecrementBannerCount()');deletedModal.SetTitle("Alert");$('#Timer_ErrorModal').css("background-color","#ffffff");$('<span style="float:right;"></span><a href="javascript:;" style="float:right;" class="VVLG mL15 mB10 Button" onclick="Timer_Close();UITools.Hide(\'Timer_ErrorModal\');Timer_TimerDecrementBannerCount();">OK</a>').appendTo('#Timer_ErrorModal');deletedModal.SetBody("An error occurred. Please reload the timers");UITools.ShowAtCenterScreen('Timer_ErrorModal',true);};function TimerValidateChangedTime(changedTime){var test=MathTools.FormatHoursAndMinutesString(changedTime);var len=changedTime.length;var arr=changedTime.split(':');for(var i=0;i<arr.length;i++){if(!Timer_checkIsInt(parseInt(arr[i],10))){Timer_ChangeValidationMessage();return false;}}var HHValidation=true;var MMValidation=true;var SSValidation=true;if(len==8||len==7){HHValidation=(arr[0]<100)?true:false;MMValidation=(arr[1]<60)?true:false;SSValidation=(arr[2]<60)?true:false;}else if(len==5){MMValidation=(arr[0]<60)?true:false;SSValidation=(arr[1]<60)?true:false;}else if(len==2){SSValidation=(arr[0]<60)?true:false;}else{Timer_ChangeValidationMessage();return false;}if(!HHValidation||!MMValidation||!SSValidation){Timer_ChangeValidationMessage();return false;}return true;};function Timer_ChangeValidationMessage(){UITools.ShowAlertBox("Please use formats: HH:MM:SS or MM:SS or SS. \r\nWith HH < 99, MM < 60, SS < 60","Alert",true,null,null,null,"Ok",null,null,null,null,null,null);};function Timer_checkIsInt(n){return+n===n&&!(n%1);};function Timer_ConvertHHMMSSToSeconds(changedTimer){if(changedTimer[0]==':'){changedTimer="0"+changedTimer+":00";}if((changedTimer.match(/:/g)||[]).length==1){changedTimer=changedTimer+":00";}var p=changedTimer.split(':'),s=0,m=1;while(p.length>0){s+=m*parseInt(p.pop(),10);m*=60;}return s;};function Timer_ConvertSecondsToHHMMSS(totalSeconds){var hours=Math.floor(totalSeconds/3600);var minutes=Math.floor((totalSeconds-(hours*3600))/60);var seconds=totalSeconds-(hours*3600)-(minutes*60);seconds=Math.round(seconds*100)/100
var result=(hours<10?"0"+hours:hours);result+=":"+(minutes<10?"0"+minutes:minutes);result+=":"+(seconds<10?"0"+seconds:seconds);return result;};function Timer_ShowToast(timer){var toastTitle="";var toastMsg="";var options={"closeButton":true,"debug":false,"newestOnTop":false,"progressBar":false,"positionClass":"toast-bottom-right","preventDuplicates":false,"onclick":function(){Timer_InitTimer();},"showDuration":"300","hideDuration":"1000","timeOut":"4000","extendedTimeOut":"1000","showEasing":"swing","hideEasing":"linear","showMethod":"fadeIn","hideMethod":"fadeOut"};toastMsg=timer.Name;if(timer.ExistingToggled){if(timer.TimerStartDateTimeUTC){toastTitle="Started Timer";}else{toastTitle="Stopped Timer";}}else{toastTitle="Created and Started Timer";}toastr.options=options;toastr["info"](toastMsg,toastTitle);var iconTimer=pi.UI.Img.Timer;$('#toast-container > .toast-info').css('background-image','url('+iconTimer+')');$('#toast-container > .toast-info').css('background-size','45px');};function Timer_ToGuid(str){return str.slice(0,8)+"-"+str.slice(8,12)+"-"+str.slice(12,16)+"-"+str.slice(16,20)+"-"+str.slice(20,str.length+1)};function Timer_TimerIncrementBannerCount(){var bannerCount=parseInt($('.banner-item-count').html());bannerCount=bannerCount+1;Timer_SetBannerCount(bannerCount);};function Timer_TimerDecrementBannerCount(){var bannerCount=parseInt($('.banner-item-count').html());bannerCount=bannerCount-1;Timer_SetBannerCount(bannerCount);};function Timer_SetBannerCount(count){if(count!=null&&count>0){$('.banner-item-count').html(count);$('.banner-item-count').show();}else{$('.banner-item-count').hide();}};function Timer_ResetVisibiltyChange(timerId){if($('#Timer_'+timerId).length){var act=$('#Timer_'+timerId)[0].getAttribute('active');act=(act=="false")?false:true;act?$('#TimerReset_'+timerId).css('visibility','hidden'):$('#TimerReset_'+timerId).css('visibility','visible');}};function Timer_ToggleStartStop(timerId,name,project){Timer_ResetVisibiltyChange(timerId);Timer_RestToggleTimer(timerId,name,project);};function Timer_TimerStart(timerId){Timer_timerId=Timer_intervalDict[timerId];if(!Timer_timerId||Timer_timerId>0){Timer_RestStartTimer(timerId);}};function Timer_TimerStop(timerId,name,project,showAddTimeEntryForm){Timer_timerId=Timer_intervalDict[timerId];if(Timer_timerId){clearInterval(Timer_timerId);Timer_timerId=undefined;Timer_RestStopTimer(timerId,name,project,showAddTimeEntryForm);}else if(showAddTimeEntryForm){Timer_RenderAddTimeForm(timerId,name,project);}};function Timer_Reset(timerId,name,project){Timer_TimerStop(timerId,name,project);Timer_ClearTimerRow(timerId);};function Timer_ClearTimerRow(timerId){var confirm=UITools.ShowAlertBox("Are you sure you want to delete this timer? The action cannot be undone.","Delete Timer",false,"","","Timer_ClearTimerRowConfirmed('"+timerId+"')","confirm","","","","");};function Timer_ClearTimerRowConfirmed(taskId){$('#timer_'+taskId).css('display','none');Timer_totalTimeS=Timer_elapsedH=Timer_elapsedM=Timer_elapsedS=0;Timer_RestClearTimer(taskId);Timer_TimerDecrementBannerCount();};function Timer_Close(){Timer_ClearAllIntervals();Timer_totalTimeS=Timer_elapsedH=Timer_elapsedM=Timer_elapsedS=0;Timer_intervalDict={};UITools.Hide('Timer_TimerModal');$('#Timer_TimerModal').remove();};function Timer_ClearAllIntervals(resetActive){if(resetActive){for(var i=0;i<Object.keys(Timer_intervalDict).length;i++){var taskId=Object.keys(Timer_intervalDict)[i];Timer_TimerStop(taskId);$('#TimerReset_'+taskId).css('visibility','visible');if($('.'+taskId)[0]){$('.'+taskId)[0].setAttribute('active',"false");}}}for(var i=0;i<Object.keys(Timer_intervalDict).length;i++){clearInterval(Timer_intervalDict[Object.keys(Timer_intervalDict)[i]]);}};function Timer_AddNewTimerClose(isX){UITools.Hide('Timer_AddNewTimerModal');$('#Timer_AddNewTimerModal').remove();if(!isX){Timer_Close();Timer_TimerIncrementBannerCount();}};function Timer_ChangeTimerClose(timeChanged,timerId){if(!timeChanged){var changedTime=$('#Timer_changeTimepicker').val();changedTime=MathTools.FormatHoursAndMinutesString(changedTime,0,0);sChangedTimer=Timer_ConvertHHMMSSToSeconds(changedTime);Timer_totalTimeS=sChangedTimer;Timer_RestSetTimer(timerId,sChangedTimer);changedTime=Timer_ConvertSecondsToHHMMSS(Timer_totalTimeS);$('#Timer_'+timerId).text(changedTime);}UITools.Hide('Timer_ChangeTimerModal');$('#Timer_ChangeTimerModal').remove();};function Timer_AddNewTimer(){var Timer_AddNewTimerModal=pi.UI.CreateDraggableLayer('Timer_AddNewTimerModal',false,null,true,'Timer_AddNewTimerClose(true);');Timer_AddNewTimerModal.SetTitle("Add Timer");Timer_AddNewTimerModal.SetBody('<div style="width:450px;"><div id="Timer_AddNewTimer" style="width:100%;max-width:100%;" data-iframe-src="'+Root+'/timer/timer-add" data-iframe-height="600" data-iframe-width="440"></div></div>');UITools.LoadMetafuseIFrame("Timer_AddNewTimer");UITools.ShowAtCenterScreen('Timer_AddNewTimerModal',true);Timer_Close();};function Timer_DetermineTime(info){var time;if(info.Success){if(info.Json.length>0){$.each(info.Json,function(index,value){time=(value.TimerPendingSeconds!=null&&value.TimerPendingSeconds>0)?value.TimerPendingSeconds:0;Timer_isActiveTimer=(value.TimerStartDateTimeUTC!=null)?true:false;Timer_timerToggle=Timer_isActiveTimer;if(time>0||Timer_isActiveTimer){if(Timer_isActiveTimer){var startTime=new Date(value.TimerStartDateTimeUTC).getTime();var serverCurrentTime=new Date(value.CurrentDateTimeUTC).getTime();var timeDiff=serverCurrentTime-startTime;if(timeDiff<0){serverCurrentTime=startTime;}Timer_totalTimeS=time+((serverCurrentTime-startTime)/1000);Timer_SetTime();Timer_UpdateTimerTime(value.Id);}else{Timer_totalTimeS=time;Timer_SetTime();}}Timer_DisplayTaskTimer(value,null);});}else{if($('#Timer_noActiveTimersMsg').length<=0){if(Timer_taskTimerModal!=null){Timer_taskTimerModal.SetBody('<div id="Timer_noActiveTimersMsg" style="width:300px" class="VVLG">No Active Timers</div>');$('#Timer_TimerModal_Options').append('<a id="AddTimerToTask" class="VVLG FL mL15" href="javascript:;" onclick="Timer_AddNewTimer()">Add Timer</a></td><td class="pR15 pB10"><a class="FR Button" href="javascript:;" onclick="Timer_Close()">OK</a>');}}}}else{var sb='';$.each(info.Errors,function(index,value){sb+=value.ErrorMessage+'\n';});pi.UI.ShowAlertBox(sb,"Oops got an Error, Sorry!",null,null,null,null,"OK");}};function Timer_SetTime(){var hours=Math.floor(Timer_totalTimeS/3600);var minutes=Math.floor((Timer_totalTimeS-(hours*3600))/60);var seconds=Timer_totalTimeS-(hours*3600)-(minutes*60);Timer_elapsedS=(seconds<1)?0:parseInt(seconds);Timer_elapsedM=(minutes<1)?0:parseInt(minutes);Timer_elapsedH=(hours<1)?0:parseInt(hours);};function Timer_DisplayTaskTimer(Timer_timerInfo,msg){msg=(msg==null)?"No active Timers to display":msg;if(Timer_timerInfo==null){if($('#Timer_noActiveTimersMsg').length<=0){Timer_taskTimerModal.SetBody('<div id="Timer_noActiveTimersMsg" style="width:300px;max-width:90%;" class="VVLG">No Active Timers</div>');}}else{var timerBody=GetElementById("TaskTimerModal_body");var timerTable=GetElementById("Timer_TimerDisplayTable");if(!timerTable){$(timerBody).children().remove();$('<table class="ViewTable W100" id="Timer_TimerDisplayTable"></table>').appendTo(timerBody);timerTable=GetElementById("Timer_TimerDisplayTable");}var row=GetElementById("timer_"+Timer_timerInfo.Id);if(row){$(row).remove();}var elaspedTime=Timer_GetElapsedTime();var iconTimer=pi.UI.Img.Timer;var iconTimerStart=pi.UI.Img.TimerStart;var iconTimerReset=pi.UI.Img.TimerReset;var iconSubmit=pi.UI.Img.Submit;var iconAdd=pi.UI.Img.Add;var roundToJS="";if(Timer_timerInfo.RoundToMinutes&&Timer_timerInfo.RoundToDirection){roundToJS=","+Timer_timerInfo.RoundToMinutes.toString()+","+Timer_timerInfo.RoundToDirection;}var timerAnchor="";if(Timer_timerInfo.EnableTimeEdit){timerAnchor='<a href="javascript:;" id="Timer_'+Timer_timerInfo.Id+'" active="'+Timer_isActiveTimer+'" onclick="Timer_RenderChangeTimerModal(\''+Timer_timerInfo.Id+'\',\''+Timer_timerInfo.UrlIcon+'\',\''+Timer_timerInfo.Url+'\',\''+escape(Timer_timerInfo.Name)+'\''+roundToJS+')'+'">'+elaspedTime+'</a>';}else{timerAnchor='<a href="javascript:;" id="Timer_'+Timer_timerInfo.Id+'" active="'+Timer_isActiveTimer+'">'+elaspedTime+'</a>';}var row='<tr id="timer_'+Timer_timerInfo.Id+'">'+'<td><a onmousedown="hl(this);" href="'+Timer_timerInfo.Url+'"><img class="icon" src="'+Timer_timerInfo.UrlIcon+'" /></a></td>'+'<td class="AL"><a onmousedown="hl(this);" href="'+Timer_timerInfo.Url+'">'+Timer_timerInfo.Name+'</a></td>'+'<td class="pL10 hidden-small"><a onmousedown="hl(this);" href="'+Timer_timerInfo.UrlProjectOrCompany+'">'+Timer_timerInfo.NameProjectOrCompany+'</a></td>'+'<td class="counter VVLG AR pL20 pR20" style="cursor:pointer">'+timerAnchor+'</td>'+'<td class="Admin AR">';row+='<a href="javascript:;" onclick="Timer_ToggleStartStop('+'\''+Timer_timerInfo.Id+'\',\''+escape(Timer_timerInfo.Name)+'\',\''+escape(Timer_timerInfo.NameProjectOrCompany)+'\')" class="icon-m"><img src="'+iconTimerStart+'" alt="Start Timer" title="Start/Stop Timer" /></a>';if(Timer_timerInfo.EnableTimeClear){row+='<a id="TimerReset_'+Timer_timerInfo.Id+'" href="javascript:;" onclick="Timer_Reset('+'\''+Timer_timerInfo.Id+'\',\''+escape(Timer_timerInfo.Name)+'\',\''+escape(Timer_timerInfo.NameProjectOrCompany)+'\')" class="icon-m"><img  src="'+iconTimerReset+'" title="Clear Timer" /></a>';}row+='<a href="javascript:;" onclick="Timer_TimerStop(\''+Timer_timerInfo.Id+'\',\''+escape(Timer_timerInfo.Name)+'\',\''+escape(Timer_timerInfo.NameProjectOrCompany)+'\',true)" class="icon-m"><img src="'+iconAdd+'" title="Enter Time" /></a>';row+='</td></tr>';$(row).appendTo(timerTable);Timer_ResetVisibiltyChange(Timer_timerInfo.Id);}$('#timerLoadingDiv').css('display','none');};function Timer_GetElapsedTime(){var displayElapsedH=("0"+Timer_elapsedH).slice(-2);var displayElapsedM=("0"+Timer_elapsedM).slice(-2);var displayElapsedS=("0"+Timer_elapsedS).slice(-2);var displayedTime;var displayFull=true;if(!displayFull){displayElapsedH=(displayElapsedH=="00")?"":displayElapsedH+":";displayElapsedM=(displayElapsedM=="00"&&displayElapsedH=="")?"":displayElapsedM+":";displayedTime=displayElapsedH+displayElapsedM+displayElapsedS;}else{displayedTime=displayElapsedH+":"+displayElapsedM+":"+displayElapsedS;}return displayedTime;};function Timer_UpdateTimerTime(timerId){var initialSeconds=Timer_ParseSelectedTaskElapsedTime(timerId,false);var actualElapsedSeconds=0,timeDifference=0,tolerance=2;var verificationStartTime=new Date().getTime();Timer_timerId=setInterval(function(){if(isNaN(initialSeconds)){initialSeconds=Timer_ParseSelectedTaskElapsedTime(timerId,false);}actualElapsedSeconds=new Date().getTime()-verificationStartTime;actualElapsedSeconds=parseInt(Math.floor(actualElapsedSeconds/100)/10);timeDifference=parseInt(((actualElapsedSeconds+initialSeconds)-Timer_totalTimeS));Timer_FixSlippingTime(initialSeconds,actualElapsedSeconds,timerId);Timer_ParseSelectedTaskElapsedTime(timerId,true);Timer_elapsedS++;Timer_totalTimeS++;if(Timer_elapsedS>=60){Timer_elapsedS=0;Timer_elapsedM++;if(Timer_elapsedM>=60){Timer_elapsedM=0;Timer_elapsedH++;}}$('#Timer_'+timerId).text(Timer_GetElapsedTime());},1000);Timer_intervalDict[timerId]=Timer_timerId;};function Timer_FixSlippingTime(initialSeconds,actualElapsedSeconds,timerId){Timer_totalTimeS=initialSeconds+actualElapsedSeconds;Timer_SetTime();$('#Timer_'+timerId).text(Timer_GetElapsedTime());};function Timer_ParseSelectedTaskElapsedTime(timerId,isReset){var localElaspsedS=0,localElapsedM=0,localElapsedH=0;var counterTimeSplit=$('#Timer_'+timerId).text().split(':');if(counterTimeSplit[2]!=null){localElapsedH=counterTimeSplit[0];localElapsedM=counterTimeSplit[1];localElaspsedS=counterTimeSplit[2];}else if(counterTimeSplit[1]!=null){localElapsedH=0;localElapsedM=counterTimeSplit[0];localElaspsedS=counterTimeSplit[1];}else if(counterTimeSplit[0]!=null){localElapsedH=0;localElapsedM=0;localElaspsedS=counterTimeSplit[0];}else{localElapsedH=localElapsedM=localElaspsedS=0;}if(isReset){Timer_elapsedH=localElapsedH;Timer_elapsedM=localElapsedM;Timer_elapsedS=localElaspsedS;}else{return parseInt(localElaspsedS)+(parseInt(localElapsedM)*60)+(parseInt(localElapsedH)*3600);}};function Timer_RestGetTimers(updateCountOnly){Timer_clientBase.get(Root+'/api/timer/list-by-current-user',null,function(res){var count=0;if(res!=null&&res.Success){if(res.Json!=null&&res.Json.length){count=res.Json.length;}if(updateCountOnly){Timer_SetBannerCount(count);}else{Timer_ClearAllIntervals();Timer_DetermineTime(res);Timer_SetBannerCount(count);}}else{Timer_RenderErrorModal();}},'','');};function Timer_RestGetTaskToast(taskId,toCreate,toStart,toStop,Timer_timerInfo){Timer_clientBase.get(Root+'/api/task',taskId,function(taskData){Timer_ShowToast(taskData,toCreate,toStart,toStop,Timer_timerInfo)},'','');};function Timer_RestStopTimer(timerId,name,project,showAddTimeEntryForm){Timer_clientBase.post(Root+'/api/timer/stop-timer?timerId='+timerId,null,function(res){if(!res.Success){Timer_RenderErrorModal();}else{if(res.Json.SavedId!=null&&res.Json.SavedId!=""){var timerRow=GetElementById("timer_"+timerId);if(timerRow){$(timerRow).remove();}Timer_RenderAddTimeForm(null,name,project,res.Json.SavedId);}else if(showAddTimeEntryForm){Timer_RenderAddTimeForm(timerId,name,project);}Timer_RestGetTimers();}},null);};function Timer_RestClearTimer(timerId){Timer_clientBase.post(Root+'/api/timer/clear-timer?timerId='+timerId,null,function(res){if(!res.Success){Timer_RenderErrorModal();}},null);};function Timer_RestCreateTimerForItem(objectId){Timer_clientBase.post(Root+'/api/timer/create-timer-for-item?itemId='+objectId+'&toggleExisting=true',null,function(res){if(!res.Success){Timer_RenderErrorModal();}else{if(res.Json){var timer=res.Json;Timer_ShowToast(timer);}Timer_RestGetTimers();}},null);};function Timer_RestStartTimer(timerId){Timer_clientBase.post(Root+'/api/timer/start-timer?timerId='+timerId,null,function(res){if(!res.Success){Timer_RenderErrorModal();}else{Timer_RestGetTimers();}},null);};function Timer_RestToggleTimer(timerId,name,project){Timer_clientBase.post(Root+'/api/timer/toggle-timer?timerId='+timerId,null,function(res){if(!res.Success){Timer_RenderErrorModal();}else{if(res.Json.SavedId!=null&&res.Json.SavedId!=""){var timerRow=GetElementById("timer_"+timerId);if(timerRow){$(timerRow).remove();}Timer_RenderAddTimeForm(null,name,project,res.Json.SavedId);}Timer_RestGetTimers();}},null);};function Timer_RestSetTimer(timerId,pendingSeconds){Timer_clientBase.post(Root+'/api/timer/set-timer?timerId='+timerId+"&pendingSeconds="+pendingSeconds,null,function(res){},null);};function HideTheVeil(id){metafuseVeil.HideVeilUnderElement(id);};