#!/usr/bin/env python3

import html;

def dir_list(path):
    r = []
    try:
        displaypath = urllib.parse.unquote(path,
                                           errors='surrogatepass')
    except UnicodeDecodeError:
        displaypath = urllib.parse.unquote(path)
    displaypath = html.escape(displaypath, quote=False)
    enc = sys.getfilesystemencoding()
    title = 'Directory listing for %s' % displaypath
    r.append('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" '
             '"http://www.w3.org/TR/html4/strict.dtd">')
    r.append('<html>\n<head>')
    r.append('<meta http-equiv="Content-Type" '
             'content="text/html; charset=%s">' % enc)
    r.append('<title>%s</title>\n</head>' % title)
    r.append('<body>\n<h1>%s</h1>' % title)
    r.append('<hr>\n<ul>')
    for name in reversed(sorted(os.listdir(path))):
        fullname = os.path.join(path, name)
        if not ".png" in name:
            continue;
        displayname = linkname = name
        # Append / for directories or @ for symbolic links
        if os.path.isdir(fullname):
            displayname = name + "/"
            linkname = name + "/"
        if os.path.islink(fullname):
            displayname = name + "@"
            # Note: a link to a directory displays with @ and links with /
        r.append('<li><a href="%s">%s</a></li>'
                 % (urllib.parse.quote(linkname,
                                       errors='surrogatepass'),
                    html.escape(displayname, quote=False)))
        r.append('</ul>\n<hr>\n</body>\n</html>\n')
    return '\n'.join(r);

def main():
    pass;

if __name__ == "__main__":
    main();
