

Paraview Plugin
---------------
To build the plugin, configure StratifiedSampling with BUILD_PARAVIEW_PLUGIN=ON.
To load the plugin in Paraview:
- Go to Tools -> Manage Plugins
- Click "Load New" under "Local Plugins"
- Browse to the build directory of StratifiedSampling and select libStratifiedSamplingPlugin.so
- Click "OK"
- Click "Close"
- To test out the plugin, create a sphere source by going to Sources -> Sphere and then click "Apply"
- Change the representation to Points
- Go to Filters -> Alphabetical -> StratifiedSampling, then click Apply
- You will see that the sphere has been significantly upsampled and that the samples are quite uniform
