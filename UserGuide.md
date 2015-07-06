# User Guide #



_**Note:** If you use one of the pre-compiled binaries from the project's website, you might still have to install some additional libraries which are not included with Actracktive. Please read the [build instructions](Building.md) for further information._

Actracktive is designed to be used as a process running in the background, doing some processing task until it is stopped. However, to tweak some parameters on the fly, it also offers an interactive GUI that shows what is currently happening in the application and allows for adjustment of various parameters and settings.

When starting the application without any command line parameters and/or configuration file, it will by default start up in interactive GUI mode. It will try to load the configured processing graph config file (named `processing-graph.xml` by default) from the application's _data directory_. If a processing graph could be successfully loaded, it will immediately start processing.

# Graphical User Interface #

The interface is divided into four regions:

  * The global control panel at the top allows to control the application as a whole:
    * **Quit** the application
    * **Start/Stop** processing
    * **Save** the settings of all nodes to the configuration file
  * The list of all nodes in the graph at the left allows to view all nodes and adjust settings (if any). The settings of each node can be shown by clicking on the node's name (with the small box in front of it). Some nodes also have a graphical representation (mostly image source/filter nodes), which is shown above the nodes name as a small preview. Clicking on this preview will show this representation in a larger version.
  * The large node display area at the right/center allows to view a larger graphical representation of a node, if it has one.
  * The status bar at the bottom of the application window will show the current status of the application and periodically updated performance data:
    * At which rate frames are actually processed, measured in Hertz (Hz)
    * How long a single frame of execution took, measured in milliseconds (ms)
    * How much time was spent being idle, also measured in milliseconds (ms)

The UI does not offer any way to remove or add any nodes the graph. This has to be done in the graph's XML configuration file. For details see the section 'Configuration' below.

# Daemon/Console Mode #

For simply executing a given processing graph without any need for adjustments, Actracktive offers also an non-interactive mode, also called _daemon mode_ or _headless mode_. To start into this mode, either a command line parameter can be passed in or it can be set via the global configuration file in the data directory. On the command line, headless mode can be enabled by using the `--headless` option, e.g.

```
$ ./Actracktive --headless
```

_Note: On Mac OS X, the application is packaged as an application bundle, which cannot be directly called with command line parameters, thus you have to go into the directory `Actracktive.app/Content/MacOS` where the application binary is located._

The application will then start in the same way as in interactive mode, except that it does not open any GUI windows. Depending on the logging configuration, log messages might be written to the console, including periodical performance data.

A complete list of command line options and their meaning can be obtained by passing in the `--help` option.

# Configuration #

Actracktive can be started without any additional configuration files, but at least a non-empty processing-graph.xml should be provided, as it would be quite useless otherwise. All configuration files, log files or any additional files needed for processing are searched and stored in the _data directory_. This is by default looked for (and created if not found) in the user's home directory, i.e. `~/Actracktive Data`. Some example files that would go in this directory can be found in the `data` directory of the project's sources (see ProjectStructure).

There are generally three possible configuration files:
  * Global application configuration (`config.xml` by default)
  * Processing graph configuration (`processing-graph.xml` by default)
  * Logging configuration (`log4cplus.properties` by default)

## Global Configuration ##

The global application configuration is a simple XML file which allows to set application wide options. The default contents would look like this:

```
<?xml version="1.0" ?>
<config>
    <headless>false</headless>
    <logging-config>log4cplus.properties</logging-config>
    <graph-config>processing-graph.xml</graph-config>
    <timer-output>5</timer-output>
</config>
```

**`headless`** controls if the application starts in headless mode by default. Note that on Mac OS X, setting this to true and starting Actracktive by double clicking the bundle will eventually start the application but will lead to a constantly bouncing app icon in the dock, as no GUI window is opened.

**`logging-config`** allows to specify a different configuration file for the logging system. Note that by default, Actracktive first looks for `log4cplus.properties` in the data directory and if no file is found it falls back to loading the default logging configuration file provided in the application's _resource directory_ (this is packaged with the applications binary, on OS X within the application bundle, on Linux in the `resources` directory).

**`graph-config`** allows to specify a different processing graph configuration file.

**`timer-output`** controls how often performance data is logged in headless mode. It specifies the delay in seconds between two log messages. Setting this to zero or a negative value will disable performance data logging.

## Processing Graph Configuration ##

Processing graph configurations are stored in XML files which are also described by a DTD found in the resources directory as well as online on the project's repository.

A processing graph is described as list of all processing nodes needed for a certain task. Each node is minimally described by its type, which is a unique string naming the implementation (usually the class name). To make connections in the graph, each node can be given a unique identifier. This identifier can be used in the configuration of other nodes to wire them together. Additionally, each node can have an arbitrary set of properties, which are defined by the implementing class and are used to control the behavior of the node. There is also a mechanism for configuring binary type properties, which are used to store non-standard data types such as images.

As an example, consider the following configuration file:

```
<?xml version="1.0" ?>
<!DOCTYPE processing-graph SYSTEM "http://actracktive.googlecode.com/svn/trunk/aux/resources/processing-graph.dtd">
<processing-graph>
    <node id="camera" type="DC1394Source" name="Camera">
        <property name="cameraId">49712223530884286:0</property>
        <property name="mode">MODE_640x480_MONO8</property>
    </node>
    <node id="shading" type="ShadingCorrectionFilter">
        <property name="enabled">true</property>
        <connection name="source">camera</connection>
        <blob name="shading"><![CDATA[QkJBQkFBQD9A...]]></blob>
    </node>
</processing-graph>
```

The first node is of type `DC1394Source` (which implements access to firewire cameras supporting the IIDC protocol using libdc1394). It is also given an ID and an arbitrary name, which is purely for displaying purposes. Two properties are configured, in this case the UUID of the camera and the 'mode', controlling image resolution and color channels. Note that implementations will have default values for properties, which allows to leave out those properties where you want to use the default. However, when the graph is saved back to file using the GUI, all properties, even those using default values, will be written to the file.

The second node is of type `ShadingCorrectionFilter` (which is an image filter for correcting shading differences in images). Again, a property is configured, in this case to make sure the filter is enabled. The next element connects the 'shading' node to the 'camera' node, i.e. the 'shading' node will get passed a pointer to the 'camera' node, thus allowing it to access the images captured by the 'camera' node. Note that connections are typed, i.e. it is not possible to connect arbitrary nodes to each other. The implementation of each node defines which type it accepts as a connection. In this case, the `source` connection of `ShadingCorrectionFilter` (actually any `ImageFilter`) accepts any node that is of type `ImageSource`.

Finally, a `blob` (binary large object) is configured. These always consist of a CDATA section containing base64 encoded data. In this case, it is a 640 by 480 pixels large greyscale image describing the shading of the scene. Storing this information in the processing graph's configuration file makes them self contained and easy to move around. Otherwise, if this kind of data was stored in external files, it becomes harder to always keep the files together.

There is no limit to how many properties, connections or blobs a node can have. Also, there is no limit to how many nodes might be connected to some other node. In this example, you could easily add several copies of the 'shading' node (giving each its own, unique ID or just leaving out the ID attribute). It is also not an error to try to configure undefined properties, they will simply be ignored.

## Logging Configuration ##

Actracktive uses the [log4cplus](http://log4cplus.sourceforge.net/) logging system, which is based on the popular log4j system widely used in the Java world. The configuration is therefore using properties files. For a detailed description how to configure the logging system, see the documentation on the log4cplus and log4j projects website.
