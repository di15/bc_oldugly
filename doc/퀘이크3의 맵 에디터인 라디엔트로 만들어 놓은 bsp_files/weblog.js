var s_uid;		var s_uname;	var ref;	var doc_uid;	var src_path;

if (document.all) {
	src_path = document.all.log_script.src;
} else {
	src_path = document.getElementById('log_script').src;
}

var uid_end = src_path.lastIndexOf("uid=",src_path);
var uname_start = src_path.indexOf("uname=",src_path);
var uname_end = src_path.lastIndexOf("uname=",src_path);

s_uid=src_path.substring(uid_end+4,uname_start-1);
s_uname=src_path.substring(uname_end+6,src_path.length);

var s_url = document.URL;

if (document.referrer) {
	ref=document.referrer;
} else {
	if (opener&&typeof(opener) == "object") {
		if (typeof(opener.document) != "unknown") {
			ref = opener.document.URL;
		}
	}
	if (! ref) {
		if (typeof(parent) == "object" ) {
			if (typeof(parent.document) != "unknown") {
				ref = parent.document.referrer;
			}
		}
		if (! ref) {
			if (parent.opener&&typeof(parent.opener) == "object" ) {
				if (typeof(parent.opener.document) != "unknown") {
					ref = parent.opener.document.referrer;
				}
			}
		}
	}
}

if (! ref && opener) {
        if (typeof(opener) == "object" ) {
                if (typeof(opener.parent) == "object" ) {
                	if (typeof(opener.parent.document) != "unknown") {
                        ref = opener.parent.document.referrer;
                    }
                }
        }
}

if (document.all) {
	doc_uid=s_uid+'&udim='+window.screen.width+'*'+window.screen.height+'&uref='+ref+'&uname='+s_uname+'&url='+s_url;
   	document.all.log_script.src='http://weblog006.cafe24.com/log.php?uid='+doc_uid;
} else {
	doc_uid=s_uid+'&udim='+window.screen.width+'*'+window.screen.height+'&uref='+ref+'&uname='+s_uname+'&url='+s_url;
	var src_real = document.getElementById('log_script');
	var src2 = document.createElement("script");
	
	src_real.parentNode.insertBefore(src2,src_real);
	src2.src = 'http://weblog006.cafe24.com/log.php?uid='+doc_uid;
}

