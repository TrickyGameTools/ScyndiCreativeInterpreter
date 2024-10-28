// Lic:
// Scyndi's Creative Interpreter
// Create API header filer
// 
// 
// 
// (c) Jeroen P. Broks, 2023
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Please note that some references to data like pictures or audio, do not automatically
// fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 23.01.14
// EndLic
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