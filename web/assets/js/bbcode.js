// Copyright (c) 2008, Stone Steps Inc. 
// All rights reserved
// http://www.stonesteps.ca/legal/bsd-license/
var opentags;           // open tag stack
var crlf2br = true;     // convert CRLF to <br>?
var noparse = false;    // ignore BBCode tags?
var urlstart = -1;      // beginning of the URL if zero or greater (ignored if -1)

// aceptable BBcode tags, optionally prefixed with a slash
var tagname_re = /^\/?(?:pre|samp|code|colou?r|size|noparse|url|img|blockquote|del)$/;
//disable [b] [i] [u] [s] [q]

// color names or hex color
var color_re = /^(:?black|silver|gray|white|maroon|red|purple|fuchsia|green|lime|olive|yellow|navy|blue|teal|aqua|#(?:[0-9a-f]{3})?[0-9a-f]{3})$/i;

// numbers
var number_re = /^[\\.0-9]{1,8}$/i;

// reserved, unreserved, escaped and alpha-numeric [RFC2396]
var uri_re = /^[-;\/\?:@&=\+\$,_\.!~\*'\(\)%0-9a-z]{1,512}$/i;

// main regular expression: CRLF, [tag=option], [tag] or [/tag]
var postfmt_re = /([\r\n])|(?:\[([a-z]{1,16})(?:=([^\x00-\x1F"'\(\)<>\[\]]{1,256}))?\])|(?:\[\/([a-z]{1,16})\])/ig;

// stack frame object
function taginfo_t(bbtag, etag)
{
   this.bbtag = bbtag;
   this.etag = etag;
}

// check if it's a valid BBCode tag
function isValidTag(str)
{
   if(!str || !str.length)
      return false;

   return tagname_re.test(str);
}

//
// m1 - CR or LF
// m2 - the tag of the [tag=option] expression
// m3 - the option of the [tag=option] expression
// m4 - the end tag of the [/tag] expression
//
function textToHtmlCB(mstr, m1, m2, m3, m4, offset, string)
{
   //
   // CR LF sequences
   //
   if(m1 && m1.length) {
      if(!crlf2br)
         return mstr;

      switch (m1) {
         case '\r':
            return "";
         case '\n':
            return "<br>";
      }
   }

   //
   // handle start tags
   //
   if(isValidTag(m2)) {
      // if in the noparse state, just echo the tag
      if(noparse)
         return "[" + m2 + "]";

      // ignore any tags if there's an open option-less [url] tag
      if(opentags.length && opentags[opentags.length-1].bbtag == "url" && urlstart >= 0)
         return "[" + m2 + "]";

      switch (m2) {
         case "code":
            opentags.push(new taginfo_t(m2, "</code></pre>"));
            crlf2br = false;
            return "<pre><code>";

         case "pre":
            opentags.push(new taginfo_t(m2, "</pre>"));
            crlf2br = false;
            return "<pre>";

         case "color":
         case "colour":
            if(!m3 || !color_re.test(m3))
               m3 = "inherit";
            opentags.push(new taginfo_t(m2, "</span>"));
            return "<span style=\"color: " + m3 + "\">";

         case "size":
            if(!m3 || !number_re.test(m3))
               m3 = "1";
            opentags.push(new taginfo_t(m2, "</span>"));
            return "<span style=\"font-size: " + Math.min(Math.max(m3, 0.7), 3) + "em\">";

         case "s":
            opentags.push(new taginfo_t(m2, "</span>"));
            return "<span style=\"text-decoration: line-through\">";

         case "noparse":
            noparse = true;
            return "";

         case "url":
            opentags.push(new taginfo_t(m2, "</a>"));
            
            // check if there's a valid option
            if(m3 && uri_re.test(m3)) {
               // if there is, output a complete start anchor tag
               urlstart = -1;
               return "<a href=\"" + m3 + "\">";
            }

            // otherwise, remember the URL offset 
            urlstart = mstr.length + offset;

            // and treat the text following [url] as a URL
            return "<a href=\"";

         case "img":
            opentags.push(new taginfo_t(m2, "\" />"));

            return "<img style=\"width:auto\" src=\"";

         case "q":
         case "blockquote":
            opentags.push(new taginfo_t(m2, "</" + m2 + ">"));
            return m3 && m3.length && uri_re.test(m3) ? "<" + m2 + " cite=\"" + m3 + "\">" : "<" + m2 + ">";

         default:
            // [samp], [b], [i] and [u] don't need special processing
            opentags.push(new taginfo_t(m2, "</" + m2 + ">"));
            return "<" + m2 + ">";
            
      }
   }

   //
   // process end tags
   //
   if(isValidTag(m4)) {
      if(noparse) {
         // if it's the closing noparse tag, flip the noparse state
         if(m4 == "noparse")  {
            noparse = false;
            return "";
         }
         
         // otherwise just output the original text
         return "[/" + m4 + "]";
      }
      
      // highlight mismatched end tags
      if(!opentags.length || opentags[opentags.length-1].bbtag != m4)
         return "[/" + m4 + "]";

      if(m4 == "url") {
         // if there was no option, use the content of the [url] tag
         if(urlstart > 0)
            return "\">" + string.substr(urlstart, offset-urlstart) + opentags.pop().etag;
         
         // otherwise just close the tag
         return opentags.pop().etag;
      }
      else if(m4 == "code" || m4 == "pre")
         crlf2br = true;

      // other tags require no special processing, just output the end tag
      return opentags.pop().etag;
   }

   return mstr;
}

//
// post must be HTML-encoded
//
function parseBBCode(post)
{
   var result, endtags, tag;

   // convert CRLF to <br> by default
   crlf2br = true;

   // create a new array for open tags
   if(opentags == null || opentags.length)
      opentags = new Array(0);

   // run the text through main regular expression matcher
   result = post.replace(postfmt_re, textToHtmlCB);

   // reset noparse, if it was unbalanced
   if(noparse)
      noparse = false;
   
   // if there are any unbalanced tags, make sure to close them
   if(opentags.length) {
      endtags = new String();
      
      // if there's an open [url] at the top, close it
      if(opentags[opentags.length-1].bbtag == "url") {
         opentags.pop();
         endtags += "\">" + post.substr(urlstart, post.length-urlstart) + "</a>";
      }
      
      // close remaining open tags
      while(opentags.length)
         endtags += opentags.pop().etag;
   }

   return endtags ? result + endtags : result;
}