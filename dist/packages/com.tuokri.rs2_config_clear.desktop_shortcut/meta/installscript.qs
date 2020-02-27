function Component() {
}

Component.prototype.createOperations = function () {
    component.createOperations();
    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut",
            "@TargetDir@/rs2-config-clear.exe",
            "@DesktopDir@/RS2 Config Clear Tool.lnk",
            "workingDirectory=@TargetDir@",
            "iconPath=%SystemRoot%/system32/SHELL32.dll",
            "iconId=22",
            "description=Start RS2 Config Clear Tool");
    }
}
