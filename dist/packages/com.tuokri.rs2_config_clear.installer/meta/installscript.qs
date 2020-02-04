//

function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut",
            "@TargetDir@/data/rs2-config-clear.exe",
            "@StartMenuDir@/rs2-config-clear.lnk",
            "workingDirectory=@TargetDir@/data/",
            "iconPath=%SystemRoot%/system32/SHELL32.dll",
            "iconId=22",
            "description=Start rs2-config-clear tool");
    }
}
