////This needs to be overridden as opposed to just set initially buecause this offset is calculated value. Need to let kendo set it initially
function MoveEditorToolbar()
{
	////this does not work if multiple editors on one page
	//$('.k-widget.k-window.k-window-titleless.k-editor-widget').offset(
	//		{ top: $('.k-widget.k-window.k-window-titleless.k-editor-widget').offset().top + 7 }
	//	);

	////this works for multiple
	$('.k-widget.k-window.k-window-titleless.k-editor-widget').css('margin-top', '8px');

};
