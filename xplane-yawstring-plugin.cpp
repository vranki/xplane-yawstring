// Based on https://developer.x-plane.com/code-sample/instanced-drawing-sample/

#include <XPLMInstance.h>
#include <XPLMDisplay.h>
#include <XPLMGraphics.h>
#include <XPLMMenus.h>
#include <XPLMUtilities.h>
#include <XPLMPlugin.h>
#include <XPLMDataAccess.h>
#include <cstdio>
#include <cstring>

#if !XPLM300
#error This plugin requires version 300 of the SDK
#endif

const char * g_objPath = "lib/airport/vehicles/pushback/tug.obj";
XPLMObjectRef g_object = NULL;
XPLMInstanceRef g_instance = {NULL};

static void load_cb(const char * real_path, void * ref)
{
    XPLMObjectRef * dest = (XPLMObjectRef *) ref;
    if(*dest == NULL)
    {
        *dest = XPLMLoadObject(real_path);
    }
}

static void menu_cb(
        void *inMenuRef,
        void *inItemRef)
{
    if(!g_object)
    {
        XPLMLookupObjects(g_objPath, 0, 0, load_cb, &g_object);
    }
    if(g_object)
    {
        const char * drefs[] = { "sim/graphics/animation/ground_traffic/tire_steer_deg", NULL };
        if(!g_instance)
        {
            g_instance = XPLMCreateInstance(g_object, drefs);
        }
    }

    static XPLMDataRef x = XPLMFindDataRef("sim/flightmodel/position/local_x");
    static XPLMDataRef y = XPLMFindDataRef("sim/flightmodel/position/local_y");
    static XPLMDataRef z = XPLMFindDataRef("sim/flightmodel/position/local_z");
    static XPLMDataRef heading = XPLMFindDataRef("sim/flightmodel/position/psi");
    static XPLMDataRef pitch = XPLMFindDataRef("sim/flightmodel/position/theta");
    static XPLMDataRef roll = XPLMFindDataRef("sim/flightmodel/position/phi");
    static XPLMDataRef yaw = XPLMFindDataRef("sim/flightmodel/position/beta");

    XPLMDrawInfo_t		dr;
    dr.structSize = sizeof(dr);
    dr.x = XPLMGetDataf(x);
    dr.y = XPLMGetDataf(y);
    dr.z = XPLMGetDataf(z);
    dr.pitch = XPLMGetDataf(pitch);
    dr.heading = XPLMGetDataf(heading) + XPLMGetDataf(yaw);
    dr.roll = XPLMGetDataf(roll) + XPLMGetDataf(yaw);

    if(g_instance)
    {
        static float dummy = 0;
        XPLMInstanceSetPosition(g_instance, &dr, &dummy);
    }
}


PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc)
{
    std::strcpy(outName, "Yaw String plugin");
    std::strcpy(outSig, "org.vranki.yawstring");
    std::strcpy(outDesc, "Draws yaw string always in 3D");

    int my_slot = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "yawstring", NULL, 0);
    XPLMMenuID m = XPLMCreateMenu("yawstring", XPLMFindPluginsMenu(), my_slot, menu_cb, NULL);
    XPLMAppendMenuItem(m, "add string", NULL, 0);
    return 1;
}

PLUGIN_API void	XPluginStop(void)
{
    if(g_instance)
        XPLMDestroyInstance(g_instance);
    if(g_object)
        XPLMUnloadObject(g_object);
}

PLUGIN_API int XPluginEnable(void)
{
    return 1;
}

PLUGIN_API void XPluginDisable(void) {}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID, long, void*) { }
