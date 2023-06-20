# Defold 3D Software Renderer extension

This is based on Defolds 3D simple project. An exension has been added that renders 3D software renderer to a 3D textured quad in Defold. 

The software render is quite simple, but supports excellent blinn and pbr rendering. Looking quite amazing for a software renderer. I have heavily modified this brilliant renderer found here:

https://github.com/zauonlok/renderer

The work done on this renderer is brilliant. Its been a great toolkit to try out some new ideas of intermixed rendering systems and some other undisclosed projects Im trying out as well. 

An example of the system rendering the ponycar with the sphere and cube from the original Defold sample.

![alt text](https://github.com/dlannan/defold-sw-renderer/blob/main/screenshots/2023-06-20_21-28-pnycar.png)

## Future
There are a few additions that I will sort out:
- animations (they are supported in the sw renderer, the need exposure to lua)
- render management (add in the ability to swithc pbr and blinn)
- better scene setup handling (root transform is embedded atm)
- SSE or AVX performance improvements for the triangle rasterization (should be fairly simple to do)
- Other pipelining improvements
- Eventual OpenGL upgrade in part (this may not happen if CPU is enough for my use case)

## License
This is MIT as is the software renderer. 


