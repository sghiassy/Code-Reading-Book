Visual Component Framework 0.2.6 

Readme

Known issues:

There may be path problems when trying to run executable that link with the VCF Runtime DLL's ( FoundationKit.dll, GraphicsKit.dll, and ApplicationKit.dll). These require a path variable called VCF_BIN to be added to your system PATH variable. The installer adds this to your user PATH but if this does not work then it will probably need to added to you local PATH. For this you'll need Administrator privileges. If you know of a better solution please contact me(ddiego@one.net) !  

Add-ins sometimes do not get added automatically to Dev Studio. These can be added manually by clicking the "Tools > Customize..." menu item and selecting the "Add-ins and macro files" tab page. Click on the "Browse..." button and look in the vcf\build\vc60\add-ins directory and then go to the specific add in project you want to add (generally VCFNewClassWiz\Release\VCFNewClassWiz.dll for now). Select the dll and click OK. The add-in will be plugged into Dev Studio for you. If there are no dlls then just build the add and repeat the previous steps. 


People experiencing any problems during or after the 
install should go to VCF Bugs(http://sourceforge.net/tracker/?func=add&group_id=6796&atid=106796) 
and submit a bug report.
