# SpeedCrunch
SpeedCrunch is a high-precision scientific calculator. It features a syntax-highlighted scrollable display and is designed to be fully used via keyboard. Some distinctive features are auto-completion of functions and variables, a formula book, and quick insertion of constants from various fields of knowledge. Available for Windows, OS X, and Linux in a number of languages. For binary downloads, please visit the [home](http://speedcrunch.org/) or [download](https://bitbucket.org/heldercorreia/speedcrunch/downloads) pages. These are the official mirrors for development: [GitHub](https://github.com/speedcrunch), [Google Code](https://code.google.com/p/speedcrunch/), [Gitorious](https://gitorious.org/speedcrunch), [Bitbucket](https://bitbucket.org/heldercorreia/speedcrunch/).

## Building
To build from source, use either CMake or QMake to build the project:

    cd src && cmake . && make

or

    cd src && qmake speedcrunch.pro && make
    
In order to have your application settings stored in the same location as
the executable for portability (e.g. running from a USB drive or a directory
without requiring installation and special permissions), configure the project
as below:

    cmake -DCMAKE_CXX_FLAGS:STRING="-DSPEEDCRUNCH_PORTABLE"

or

    qmake "DEFINES+=SPEEDCRUNCH_PORTABLE" speedcrunch.pro

## Contributing
- Report bugs or request features in the [issue tracker](http://code.google.com/p/speedcrunch/issues/list).
- Add or improve a [translation](https://www.transifex.com/projects/p/speedcrunch/).
- Send a message to the [forum](https://groups.google.com/group/speedcrunch).
- Follow the news on the [blog](http://speedcrunch.blogspot.com).

## License
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
