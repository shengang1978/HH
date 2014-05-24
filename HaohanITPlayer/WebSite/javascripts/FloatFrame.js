//DRAG EVERYTHING v2.0 2/27/07
dragoffx = 0;
dragoffy = 0;
dragobj = "";
dragz = 500;
//----------------------------------
function floatdown(evt) {
    try {
        evtsrc = event.srcElement;
        evt = event;
    }
    catch (e) {
        evtsrc = evt.target;
    }

    evtsrcx = ischild(evtsrc, "Float");
    try {
        if (ischild(evtsrc, "NOMOVE") != null) {
            return;
        }
    }
    catch (e) {
    }
             
    if (dragobj == "") {
        if (evtsrcx != null) {
            dragobj = evtsrcx;
            dragoffx = ((evt.clientX || evt.x) - parseInt(dragobj.style.left, 10)) + document.body.scrollLeft;
            dragoffy = ((evt.clientY || evt.y) - parseInt(dragobj.style.top, 10)) + document.body.scrollTop;
            dragobj.style.zIndex = dragz;
        }
    }
    return false;
}
//----------------------------------
function floatmove(evt) {
    try {
        evt = event;
    } 
    catch (e) {
    }

    try {
        evtsrc = event.srcElement; evt = event;
    }
    catch (e) {
        evtsrc = evt.target;
    }

    if (evtsrc.tagName == "INPUT") {
        return true;
    }
         
    if (dragobj != "") {
        try {
            newdragx = ((evt.clientX || evt.x) - dragoffx);
            newdragy = ((evt.clientY || evt.y) - dragoffy);
            if (dragobj.xmin != undefined) {
                if ((newdragx >= dragobj.xmin) && (newdragx <= dragobj.xmax)) {
                    dragobj.style.left = newdragx + "px";
                }
            }
            else {
                dragobj.style.left = newdragx + "px";
            }
            
            if (dragobj.ymin != undefined) {
                if ((newdragy >= dragobj.ymin) && (newdragy <= dragobj.ymax)) {
                    dragobj.style.top = newdragy + "px";
                }
            }
            else {
                dragobj.style.top = newdragy + "px";
            }

            if (dragobj.report != undefined) {
                dragobjrep = dragobj.report + "(" + parseInt(dragobj.style.left, 10) + "," + parseInt(dragobj.style.top, 10) + ")";
                eval(dragobjrep);
            }
        } catch (e) { }
    }

    try {
        thisevt = event.srcElement;
    }
    catch (e) {
        thisevt = evt.target;
    }

    thisevtx = ischild(thisevt, "Float");
    if (thisevtx != null) {
        thisevt.style.cursor = "default";
    }

    return false;
}
//----------------------------------
function floatup(event) {
    if (dragobj != "") {
        dragz++;
    }
    dragobj = "";
    return false;
}
//----------------------------------
try { document.addEventListener("mousedown", floatdown, false); } 
catch (e) {document.onmousedown = floatdown; }
try { document.addEventListener("mousemove", floatmove, false); } 
catch (e) {document.onmousemove = floatmove; }
try { document.addEventListener("mouseup", floatup, false); } 
catch (e) {document.onmouseup = floatup; }
//----------------------------------

function ischild(thisobj, idtype) {
    if (thisobj.tagName == "INPUT") {
        return null;
    }
    do {
        try {
            tobj = String(thisobj.id);
        }
        catch (e) {
            tobj = "";
        }
        try {
            if (tobj.indexOf(idtype) != -1) {
                break;
            }
            thisobj = thisobj.offsetParent;
        }
        catch (e) {
            break;
        }
    }while (tobj != "BODY");
    
    return thisobj;
}
     
