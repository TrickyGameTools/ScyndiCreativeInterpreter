using System.Text;
using TrickyUnits;


// This is only a quick program which will make sure a proper header file containing all APIs is present.

try {
    QCol.Doing("Running from", Directory.GetCurrentDirectory());
    const string APIDir = "E:/Projects/Applications/Slyvina/Apps/SCI/SCI_Run/API";
    var APIS = FileList.GetDir(APIDir); if (APIS == null) throw new Exception($"I could not get the contents of \"{APIDir}\"!");
    var Headers = new StringBuilder("");
    var InitFunc = new StringBuilder("\tinline void InstallAllAPIS() {\n");

    foreach (var APISource in APIS) {
        if (qstr.ExtractExt(APISource).ToLower() == "cpp") {
            QCol.Doing("Adding", APISource);
            var APIName = qstr.StripAll(APISource);
            Headers.Append($"\tvoid Init_{APIName}();\n");
            InitFunc.Append($"\t\tInit_{APIName}();\n");
        } else {
            QCol.Doing("Skipping", APISource);
        }
    }


    InitFunc.Append("\t}\n");
    const string AO = "{", AS = "}";
    var Output = $"namespace Scyndi_CI{AO}\n\n{Headers}\n\n{InitFunc}\n\n{AS}\n";
    QCol.Green($"{Output}\n");
    QuickStream.SaveString($"{APIDir}/SCI_InstallAPIs.hpp", Output);

} catch (Exception E) {
    QCol.QuickError($"{E.Message}");
    QCol.Magenta($"\n\n{E.StackTrace}\n\n");
} finally {
    Console.ResetColor();
}