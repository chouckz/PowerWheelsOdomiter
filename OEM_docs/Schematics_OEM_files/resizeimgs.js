document.writeln("<link rel=stylesheet type=text/css href='resizeimgs.css'>");
document.writeln("<div id=showimage style='position:absolute;visibility:hidden;border:1px solid black'></div>");


// =============================================================================
// Begin user defined variables
// =============================================================================
var resizeType = 0; // 0 = Dynamically resize to a fraction of the users desktop
                    //     width divided by the dynamicDivisor variable below. 
                    //     (i.e. 1024/4 = 256 or 1280/4 = 320)
                    // 1 = Resize the image if it is wider than the fixedWidth
                    //     variable below
var enlargeType    = 0     // 0 = Opens in the same window
			               // 1 = Opens in a new window
var fixedWidth     = 650;  // Resize the image if it is wider than this many 
                           // pixels. Only used when resizeType = 1.
var dynamicDivisor = 2;    // Resize images to users screen width divided by 
                           // this number. Only used when resizeType = 0.
var alignClose   = "left"; // Values are 'left' or 'right'. Default is right.
var topSpacing   = 25;      // Spaces the pop-up window this number of pixels 
                           // from the top of the window
var leftSpacing  = 25;      // Spaces the pop-up window this number of pixels 
                           // from the left of the window
// =============================================================================
// End of user defined variables
// =============================================================================




// =============================================================================
// Do not modify below this line
// =============================================================================
var ie=document.all
var ns=document.layers
var ns6=document.getElementById&&!document.all
var nsx,nsy,nstemp

function walkImages(){
    re = /^img/;
    if (document.images){
        for (a=0;a<document.images.length;a++){
          if (re.test(document.images[a].name)) resizeImage(document.images[a]);
        }
    }
}

function doimage(imgObj,event){
    if (document.images) enlargeimg(imgObj,event);
}

//Thumbnail image viewer- 
//© Dynamic Drive (www.dynamicdrive.com)

function resizeImage(thisimage) {
    if (resizeType == 0) fixedWidth = parseInt(screen.width/dynamicDivisor);
    if (thisimage.width > fixedWidth) {
		thisimage.width = fixedWidth;
		thisimage.style.cursor='pointer';
		thisimage.title='Click the image to enlarge';
	}
}

function enlargeimg(which,e){
    if (resizeType == 0) fixedWidth = parseInt(screen.width/dynamicDivisor);
	if (which.width < fixedWidth) {return false;}
	if (enlargeType == 0) {
		if (ie||ns6){
			crossobj=document.getElementById? document.getElementById("showimage") : document.all.showimage
			if (crossobj.style.visibility=="hidden"){
				crossobj.style.left = (ns6? pageXOffset : document.body.scrollLeft) + leftSpacing
				crossobj.style.top  = (ns6? pageYOffset : document.body.scrollTop) + topSpacing
			    alignRE = /^left$/i
				crossobj.innerHTML= (alignRE.test(alignClose))? '<div class=tvhead align=left id=drag>&nbsp;<span class=tvclosex onClick=closepreview() style=cursor:pointer> X </span>&nbsp;<span class=tvclose onClick=closepreview()>Close</span> </div><img src="'+which.src+'">' : '<div class=tvhead align=right id=drag> <span class=tvclose onClick=closepreview()>Close</span>&nbsp;<span class=tvclosex onClick=closepreview() style=cursor:pointer> X </span>&nbsp;</div><img src="'+which.src+'">';
				crossobj.style.visibility="visible"
			}
			else
				crossobj.style.visibility="hidden"
				return false
		}
		else if (document.layers){
			if (document.showimage.visibility=="hide"){
				document.showimage.document.write('<a href="#" onMouseover="drag_dropns(showimage)"><img src="'+which.src+'" border=0></a>')
				document.showimage.document.close()
				document.showimage.left=e.x
				document.showimage.top=e.y
				document.showimage.visibility="show"
			}
			else
				document.showimage.visibility="hide"
				return false
		}
		else
		return true
	}
	else if (enlargeType == 1) {
			eval('window.open("showpic.asp?pic=' + which.src + '","","width='+which.width+'px,height='+which.height+'px,resizable=1,scrollbars=0")')
	}
}


function closepreview(){
	crossobj.style.visibility="hidden"
}

function drag_dropns(name){
	temp=eval(name)
	temp.captureEvents(Event.MOUSEDOWN | Event.MOUSEUP)
	temp.onmousedown=gons
	temp.onmousemove=dragns
	temp.onmouseup=stopns
}

function gons(e){
	temp.captureEvents(Event.MOUSEMOVE)
	nsx=e.x
	nsy=e.y
}
function dragns(e){
	temp.moveBy(e.x-nsx,e.y-nsy)
	return false
}

function stopns(){
	temp.releaseEvents(Event.MOUSEMOVE)
}

function drag_drop(e){
	if (ie&&dragapproved){
	crossobj.style.left=tempx+event.clientX-offsetx
	crossobj.style.top=tempy+event.clientY-offsety
	}
	else if (ns6&&dragapproved){
	crossobj.style.left=tempx+e.clientX-offsetx
	crossobj.style.top=tempy+e.clientY-offsety
	}
	return false
}

function initializedrag(e){
	if (ie&&event.srcElement.id=="drag"||ns6&&e.target.id=="drag"){
	offsetx=ie? event.clientX : e.clientX
	offsety=ie? event.clientY : e.clientY

	tempx=parseInt(crossobj.style.left)
	tempy=parseInt(crossobj.style.top)

	dragapproved=true
	document.onmousemove=drag_drop
	}
}

document.onmousedown=initializedrag
document.onmouseup=new Function("dragapproved=false")
document.onload=walkImages();
