#include "App.h"

#ifdef __WXOSX__
#include <unistd.h>
#include <CoreFoundation/CoreFoundation.h>
#endif

wxIMPLEMENT_APP(App);

App::App() {
// For MacOS
#ifdef __WXOSX__
	CFURLRef url = CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle());
	char path[PATH_MAX];
	if (!CFURLGetFileSystemRepresentation(url, true, (UInt8*)path, sizeof(path)) || chdir(path) != 0);
	CFRelease(url);
#endif
}

bool App::OnInit() {
	main_frame = new Main();
	main_frame->Show();
	return true;
}
