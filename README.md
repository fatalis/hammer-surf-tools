# Surf Tools for Hammer++ (Build 8870)

A mod of Hammer++ with surf-related features.

## Features

### Clip Angle Measurer
- When using the clipping tool, it will show how surfable the angle is.  
- **Toggle**: press `O`.

### Ramp Angle Measurer
- When selecting a solid (not an entity or group), it will show how surfable the angle is.

### AngleFix
- Select a solid (not an entity or group) and press `Ctrl + Alt + Shift + A`.

### Trigger Generator
- Creates triggers by extruding from selected faces.  
- **Usage**: select some faces with the texture tool, then press `Ctrl + Alt + Shift + T`.

### Curved Ramp Generator
- Convex ramps are not supported yet.  
- **Usage**: select a start segment that’s not rotated, then press `Ctrl + Alt + Shift + S`.

## About Angle Steepness Rating
When you see ratings like `5/10` with the angle measure tools:

- `5/10` → "Sweet spot" (~50 degrees)  
- `0/10` → Just barely surfable  
- `10/10` → Probably too steep to be fun  

## Notes
- There’s also a context menu in **Tools → Surf Tools** and in the right-click context menu.

## Installation
1. Extract `version.dll` to the directory where you have `hammerplusplus.exe`.

### Linux
- Use this environment variable:  
```bash
WINEDLLOVERRIDES="version=n,b"
