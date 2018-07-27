/// <reference path="MetafuseBase.js" />


function PageHeaderAnchorElementsToWindowRight()
{

	//make sure there is a div holding the content
	var contentSizeDiv = GetElementById("pi-left-right");
	if (contentSizeDiv == null)
	{
		return;
	};

	//determine the content width
	var contentWidth = UITools.GetElementWidth(contentSizeDiv);

	var piTopBarDiv = GetElementById("pi-banner-bg");

	//sync the top bar width to the width of the content
	if (piTopBarDiv)
	{
		$(piTopBarDiv).width(contentWidth);
	}

	var windowWidth = UITools.GetWindowWidth();
	var scrollLeft = UITools.GetScrollLeft();


	var ph_piPageTitleProject = GetElementById("ph_piPageTitleProject");
	var ph_piPageTitle = GetElementById("ph_piPageTitle");
	var ph_piPageViews = GetElementById("ph_piPageViews");

	if(ph_piPageTitleProject != null && ph_piPageTitle != null)
	{
		//reset
		var $ph_piPageTitle = $(ph_piPageTitle)
		var $ph_piPageTitleProject = $(ph_piPageTitleProject);
		

				

		$ph_piPageTitle.css('width','auto');
		$ph_piPageTitleProject.css('width','auto');



		var ptX = UITools.GetPageXCoordinate(ph_piPageTitle);
		var ptW = $ph_piPageTitle.width();//width of the page title
	
		
	
		var titleWidth = ptX + ptW;
		//alert('titleWidth' + titleWidth + " windowWidth: " + windowWidth);

		var buffer = 10;
		
		//if the page title is out of the page, then adjusting will happen
		if(titleWidth > ((windowWidth - buffer) + scrollLeft))
		{
			var targetRatio = .4;
			
			if(windowWidth < 1000)
			{
				targetRatio = .3;
			}			
			else if(windowWidth < 600)
			{
				targetRatio = .25;
			}
			

			var targetX = (windowWidth * targetRatio) + scrollLeft;

			//see where the page title projec starts
			var pptX = UITools.GetPageXCoordinate(ph_piPageTitleProject);
			
			//if the target is less than where the title starts we need to truncate the width of the project title
			var trim = titleWidth - windowWidth;

			//if the target X position of the title is greater, then trim the project title
			if(targetX < ptX)
			{	
				
				var ppW = $ph_piPageTitleProject.width();
	
				var ptrim = trim;
				
				/*alert(ppW + " " + ptrim + " " + targetX + " " + pptX);*/
			
				if((ppW - ptrim - buffer) < targetX)
				{
					ptrim = ppW - targetX;
					trim = trim - ptrim;

					/*alert('trim' + trim);*/
				}
				else
				{	
					trim = 0;
				}
				

				//if there is no trim to truncate from the other side then 
				if(trim <= 0)
				{
					ppW = ppW - ptrim - buffer + scrollLeft;
				}
				else
				{
					ppW = ppW - ptrim;
				}
				/*alert(ppW);*/
		
				$ph_piPageTitleProject.width(ppW);

			
			}
			
			//now trim the rest off the title
			
			if(trim > 0)
			{
	
				ptW = ptW - trim - buffer + scrollLeft;
				/*alert('ptW:' + ptW);*/
				$ph_piPageTitle.width(ptW);
			}

			//$(ph_piPageTitle).hide();
			
			//$(ph_piPageTitle).show();


			//alert(targetX + " " + ptX + " " + pptX);
			
			
	
		}
		
		

		//alert('in here');
	}
	else if(ph_piPageTitle != null)
	{
		var $ph_piPageTitle = $(ph_piPageTitle)		

		$ph_piPageTitle.css('width','auto');
	
		/*alert('in enterprise');*/
		var ptX = UITools.GetPageXCoordinate(ph_piPageTitle);
		var ptW = $ph_piPageTitle.width();
		
		var titleWidth = ptX + ptW;
		/*alert('titleWidth' + titleWidth + " windowWidth: " + windowWidth);*/

		var buffer = 3;

		//if the page title is out of the page, then adjusting will happen
		if(titleWidth > ((windowWidth - buffer) + scrollLeft))
		{
			//if the target is less than where the title starts we need to truncate the width of the project title
			var trim = titleWidth - windowWidth;

			ptW = ptW - trim - buffer + scrollLeft;

			$ph_piPageTitle.width(ptW);
		}

	}


	contentWidth = UITools.GetElementWidth(contentSizeDiv);

	//sync the top bar width to the width of the content
	if (piTopBarDiv)
	{
		$(piTopBarDiv).width(contentWidth);
	}

	//the rest of the content now needs to be 
	var marginRight = contentWidth - windowWidth - scrollLeft;

	var $elementsToAnchorRight = $('.AnchorElementOnRightScreen, .display-edit');
	$elementsToAnchorRight.css('right', 0);
	if (marginRight > 0)
	{
		$elementsToAnchorRight.css('right', marginRight);
	}
	
	
};

function LeftNavExpandCollapse(element, pageHeaderPrefix, postBackUniqueID)
{

	var navIsCollapsed = FormTools.GetFormElementValue(pageHeaderPrefix + "__NIC");
	var navIsOutput = FormTools.GetFormElementValue(pageHeaderPrefix + "__NIO");

	var hide = true;
	if (navIsCollapsed == "1")
	{
		hide = false;
		FormTools.SetFormElementValue(pageHeaderPrefix + "__NIC", "0");
	}
	else
	{
		FormTools.SetFormElementValue(pageHeaderPrefix + "__NIC", "1");
	}


	var piRight = GetElementById("pi-right");

	if (hide)
	{
		piRight.style.display = "block";
		$("#pi-left").hide();
		$(element).removeClass("pi-left-close");

	}
	else
	{

		piRight.style.display = "table-cell";
		$("#pi-left").show();
		$(element).addClass("pi-left-close");
	}

	//add the processing if we need to load the nav
	var elementToShowProcessingNear = null;
	if (hide == false && navIsOutput == "0")
	{
		elementToShowProcessingNear = element;
	};

	//hContentCell

	//make ajax call back
	MAPBB(postBackUniqueID, "", false, elementToShowProcessingNear, "Processing...", false, false);

	OnWindowResizeFunctions.CallFunctions();

};

//------------------------------------------------------------------------------------------------
// Search functions
//------------------------------------------------------------------------------------------------
/** 
* Object used to represent the search bar elements and functions.
*/
var PISearchBar = function ()
{
	this.ElementId = "";
	this.SearchBarId = "";
	this.AdvancedSearchButtonId = "";
};

/// create the search bar function.
var piSearchBar = new PISearchBar();

/** 
* Open the Search Bar
* @param elementId - element or id of the element clicked to open the search
* @param searchBarId - the ID of the search control
*/
function UI_OpenSearchBar(elementId, searchBarId, advancedBarId, itemId)
{
	piSearchBar.ElementId = elementId;
	piSearchBar.SearchBarId = searchBarId;
	piSearchBar.AdvancedSearchButtonId = advancedBarId;

	var $element = $("#" + elementId);
	var $searchBar = $("#" + searchBarId);
	//var searchBarWidth = $searchBar.width;

	var onclickString = "";
	if (itemId !== undefined && itemId != null && itemId != "")
	{
		onclickString = "HSearchDoSearch('" + piSearchBar.SearchBarId + "','" + itemId + "');";
	}
	else
	{
		onclickString = "HSearchDoSearch('" + piSearchBar.SearchBarId + "');";
	}

	$element.attr("onclick", onclickString);
	//$element.attr("data-search", "search"); //RC this was doing something... probably had to do with why it stopped working...
	$element.animate({ "padding-left": "+=155" }, 400, null, UI_ShowSearchBar);
};

/** 
* Shows the search UI Bar
*/
function UI_ShowSearchBar()
{
	var $element = $("#" + piSearchBar.ElementId);
	$element.css("padding-left", "0");
	UITools.Show(piSearchBar.SearchBarId, 'slide');
	UITools.Show(piSearchBar.AdvancedSearchButtonId, 'slide');
	var $searchBar = $("#" + piSearchBar.SearchBarId);
	if (BrowserTools.IsInternetExplorer)
	{
		// this is really stupid branch... but it is required for IE...
		setTimeout(function () { $searchBar.focus(); }, 10);
	}
	else
	{
		$searchBar.focus();
	}
};

/*search*/
function HSearch(element, target, searchLayerId)
{
	var searchLayer = GetElementById(searchLayerId);
	var x = UITools.GetPageXCoordinate(element);
	var y = UITools.GetPageYCoordinate(element);


	if (searchLayer == null)
	{

		var serverResponse = new MetafuseAjaxPostBackObjectServerResponse();
		serverResponse.OnAfterExecuteSuccessfulResponse = function (serverResponse)
		{
			HSearchShowLayer(searchLayerId, x, y);
		};

		Ajax.DoAjaxPostBack(target, "", false, null, "", false, true, serverResponse);

	}
	else
	{

		HSearchShowLayer(searchLayerId, x, y);
	}
};

function HSearchShowLayer(searchLayerId, x, y)
{

	var searchLayer = GetElementById(searchLayerId);

	UITools.ShowAtXY(x, y, searchLayer, false, null, -100, 25);
};

function HSearchDoSearch(elementId, itemId, checkBoxId)
{

	var searchValue = FormTools.GetFormElementValue(elementId);

	//if (itemId)
	//{
	//	searchValue += "&itemId=" + itemId;
	//}
	//if (checkBoxId)
	//{
	//	var checkBoxValue = FormTools.GetFormElementValue(checkBoxId);

	//	if (checkBoxValue == "on")
	//	{
	//		searchValue += "&keywordschildrenonly=true";
	//	}
	//}

	L("/report/search?keywords=" + searchValue);

};


function PI_ClassicNav_ToggleMainNavResponsive(e, clientID, resClientID, menuCSS, menuItemClass, widthString, heightString, speedOpen, speedClose)
{

	

	if (widthString == null && heightString == null)
	{
		widthString = "90%";
		//heightString = "100%";
	}
	
	

	if (!speedOpen)
	{
		speed = "fast";
	}


	

	//source
	var $nav = $("#" + clientID);

	//destination
	var $navresp = $("#" + resClientID);

	var speed = "fast";



	if ($navresp.children().length == 0)
	{
		

		$navresp.append("<nav><ul class=\"" + menuCSS + "\"></ul></nav>");
		$ul = $navresp.children("NAV")[0];
		var ul = $($ul).children("UL")[0];
		$ul = $(ul);
		

		ph_mo.AppendToResponsiveMenu(ul, 1, menuItemClass, 3);

	
		ph_lo.AppendToResponsiveMenu(ul, 1, menuItemClass, 3);


		//remove the reports children if there are any...
		var bannerLIs = $ul.children("LI");

		//remove the "favorites" for now
		if (bannerLIs.length > 2)
		{
			$(bannerLIs[2]).remove();
		}
		//modify the banner
		if (bannerLIs.length > 4)
		{
	
			var timer = bannerLIs[4];

			var $timer = $(timer);

			$timer.children('div.mainnav-item-child').remove();

			$timer.click(function () { Timer_InitTimer(); PI_SiteMapNav_CloseMainNavResponsive(clientID, resClientID, menuItemClass, widthString, heightString, speedOpen, speedClose); });
		
			//$(bannerLIs[4]).remove();
		}

		//}
		//print friendly
		if (bannerLIs.length > 5)
		{
			$(bannerLIs[5]).remove();
		}

		


		PI_SiteMapNav_SetupNavigationMenu(resClientID, resClientID, menuItemClass);

	}


	PI_SiteMapNav_ToggleMainNavResponsive(e, clientID, resClientID, menuItemClass, widthString, heightString, speedOpen, speedClose);



	return false;

};


function PI_ClassicNav_TogglePageMenuResponsive(e, clientID, resClientID, menuCSS, menuItemClass, widthString, heightString, speedOpen, speedClose)
{

	
	

	if (widthString == null && heightString == null)
	{
		widthString = "90%";
	}

	if (!speedOpen)
	{
		speed = "fast";
	}


	var vertical = true;

	//source
	var $nav = $("#" + clientID);

	//destination
	var $navresp = $("#" + resClientID);

	var speed = "fast";



	if ($navresp.children().length == 0)
	{
		

		$navresp.append("<nav><ul class=\"" + menuCSS + "\"></ul></nav>");
		$ul = $navresp.children("NAV")[0];
		var ul = $($ul).children("UL")[0];
		$ul = $(ul);
		

		if (isObject(window["ph_to"]) && ph_to != null)
		{
			ph_to.AppendToResponsiveMenu(ul, 1, menuItemClass, 3);
		}

		if (isObject(window["ph_bo"]) && ph_bo != null)
		{
			ph_bo.AppendToResponsiveMenu(ul, 1, menuItemClass, 3);
		}


		//remove the reports children if there are any...
		/*
		var bannerLIs = $ul.children("LI");

		//remove the "favorites" for now
		if (bannerLIs.length > 2)
		{
			$(bannerLIs[2]).remove();
		}
		//remove timer
		if (bannerLIs.length > 4)
		{
			$(bannerLIs[4]).remove();
		}
		//print friendly
		if (bannerLIs.length > 5)
		{
			$(bannerLIs[5]).remove();
		}
		*/
		


		PI_SiteMapNav_SetupNavigationMenu(resClientID, resClientID, menuItemClass);

	}


	PI_SiteMapNav_ToggleMainNavResponsive(e, clientID, resClientID, menuItemClass, widthString, heightString, speedOpen, speedClose);



	return false;

};

