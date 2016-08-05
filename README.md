# HTML_cut
Simple tool for getting content from HTML tag based on specified attributes.

## Usage
    html_cut [OPTIONS]...

File can be piped to stdin or read by program (see '-f' option).

	-f [HTML FILE]	--file=[HTML FILE] - HTML file is read by a program
	-t [TAG]	--tag=[TAG] - HTML tag is required
	-a [ATTRIBUTE]=[VALUE]	--attribute=[ATTRIBUTE]=[VALUE] (VALUE=true for empty attribute) - HTML attribute with value
	--trim - left trim spaces and tabs
	--keep - keep tags, not just the content
  --silent - disable all error messages being printed to stderr


## Example
    user@PC $ cat html_example.html
    <html>
      <div id="first">
        This is the message.
      </div>
      <div id="sec">
        Hello world!
      </div>
    </html>

    user@PC $ cat html_example.html | html_cut --trim -t div -a id=sec
    Hello world!

## License
MIT License
Copyright (c) 2016 Kacper Zybała

See LICENSE file for more information.
