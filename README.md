# CPP library that does what other libraries don't

In my programmer's life, often, I need to write my own libraries to do something more or less complex that cpp does not handle in easy mode.
For example, to find files or directory you need to handle manually recursively, search filter, case sensitive and other logic depends of kind of search, for that, I create a few group of functions that can search, filter in name, filter in extension, recursive, select files of folder, set callback for progress, and return directly the result as list, all in only one api call.
So, dpplib is an extension of cpp that helps to simplify use of base classes, containers ecc of CPP STL.

## Dependencies:

dpplib is based on C++20 and can be used also on C++17, for fully funtionality support you need to have boost al least 1.74.

## Features:

- Cross compile library: can be complied on Windows, Linux and Wasm (with emscripten but no network support).
- Can be complied as static or shared library.
- Qt framework extension addons.
- Network implementations of REST client, tcp and udp sever, uri manipulation.
- Based on cmake.

## Base modules:

| Name         | Use                                                                                                                                                                                                           | Info                                                                            |     |
| ------------ | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------- | --- |
| DChrono      | Time formatting and conversion functions                                                                                                                                                                      |                                                                                 |     |
| DCsv         | Easy read csv strings, read fileds, convert to vector                                                                                                                                                         | Can be used as stand-alone lib                                                  |     |
| DLog         | Easy create logging with color support. Static mode (only printout), or class mode that support formatting, log level, auto delete by numbers of files or by size                                             | Can be used as stand-alone lib                                                  |     |
| DPath        | Contains a lot of functions for list copy, move, count, delete multiple files or folders, recursively, filtering by name and extension. Extension change, file age check, permissions...                      |                                                                                 |     |
| DPathWatcher | An usefull class to monitor files or folders about changes, Sync or async mode                                                                                                                                |                                                                                 |     |
| DPreferences | Class for store preferences as a tree in a json file                                                                                                                                                          |                                                                                 |     |
| DSetiings    | A little extension for DPreferences that create a settings structure for an app and set data dir, log dir, station name, station id and last status string                                                    |                                                                                 |     |
| DShell       | Process start sync/async, machine shutdown                                                                                                                                                                    | Qt extention addon                                                              |     |
| DString      | All string manipulation functions that you can think: trim, cut, remove, compare, contains, starts with, end with, number conversion, format... all case sensitive controlled. String list base manipulation. |                                                                                 |     |
| DStringGrid  | Class for manipulate a string matrix using rows columns, cells. Load and save CSV format.                                                                                                                     |                                                                                 |     |
| DSyncFile    | A simple class to easy sync 2 files by last write date                                                                                                                                                        |                                                                                 |     |
| DSyncWatcher | Extends use of DSyncFile adding a thread based watching for automatic sync a list couples of source and dest files to be synched (with callbacks)                                                             |                                                                                 |     |
| DTree        | A class to rappresent a tree object with helper functions for easy read primitive or string fields. Can load and save to json format.                                                                         |                                                                                 |     |
| DUpdate      | A class that can handle a repository for app update and can automatically do the upgrade (also if program is running)                                                                                         | Form now only folder repository is support, needs to add ftp, and https support |     |
| DValue       | A class that store a list of string values used as **returned values** by some class in this lib.                                                                                                             | Need to be improved with generic template for storing any kind of data.         |     |
| DVector      | Vector helpers functions to easy manipulate vectors: split, join, remove duplicate extract key or value from map.                                                                                             |                                                                                 |     |
| DWindow      | Is a DPreferences use for store gui window position and size.                                                                                                                                                 |                                                                                 |     |

## Network modules:

| Name        | Use                                                                             | Info         |     |
| ----------- | ------------------------------------------------------------------------------- | ------------ | --- |
| DNetwork    | For now only GetHostName()                                                      |              |     |
| DRestClient | Class to create a complete fully functionally REST Client                       |              |     |
| DTcpServer  | Class to create a TCP server                                                    | Not complete |     |
| DUdpServer  | Class to create an UDP server                                                   |              |     |
| DUri        | Class for manipulating uri: encode, decode, extract parts, compose, validate... |              |     |

## Qt modules

| Name                    | Use                                                                                                                                                                     | Info |     |
| ----------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---- | --- |
| DFormLog                | QDialog implementation that extends DLog  functionality with a viewer.                                                                                                  |      |     |
| DQApp                   | QApplication helper: for now only function is setStyleSheetFromFile() that can apply a style sheet to all application loading it from file                              |      |     |
| DQCheckBoxDelegate      | QStyledItemDelegate implementation that adds a check box to a QListWidget family controls                                                                               |      |     |
| DQLabel                 | QLabel implementation that adds double click support                                                                                                                    |      |     |
| DQProcess               | QProcess implementations that adds:<br/>Handling of multple instances of same process .<br/>Callbacks for process ended and data present on stdout/stdin/stderr streams |      |     |
| DQProgressBar           | QProgressBar implementation that adds progress step handling                                                                                                            |      |     |
| DQProgressBarDelegate   | QStyledItemDelegate implementation that adds a progress bar to a QListWidget family controls                                                                            |      |     |
| DQShell                 | DShell extension that use QProcess and  adds ShowInFolder() api that open a file manager window and automatically select the file/folder                                |      |     |
| DQSpinnerWidget         | Widget that shows a rotating spinner                                                                                                                                    |      |     |
| DQSpinnerWidgetDelegate | QStyledItemDelegate implementation that adds a DQSpinnerWidget to a QListWidget family controls                                                                         |      |     |
| DQWidget                | A set of helper functions for widgets:<br/> QGridLayout: easy add, remove contents.<br/>QTabWidget: find by name.                                                       |      |     |
| DQWindow                | A set of helper functions for windows and dialogs:<br/>Enable/disable layout and its content.<br/>Save and restore window position and size.<br/>Easy show info dialog. |      |     |
| DSplahScreen            | QSplashScreen implementation that adds a usefull progress bar                                                                                                           |      |     |

## How to build:

Install or download and build boost.

Go to libdpp folder

mkdir build

cd build

cmake ..

build -j4

or simply load CmakeLists.txt file in Qt Creator IDE



## How to add to your project:
