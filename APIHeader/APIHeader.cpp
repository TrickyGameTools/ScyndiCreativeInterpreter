
// Just a quick and dirty manual conversion of the original application from C#
// When I start building for Linux as well, this is just the more convenient run

#include <SlyvQCol.hpp>
#include <SlyvStream.hpp>
#include <SlyvString.hpp>
#include <SlyvDir.hpp>

using namespace Slyvina;
using namespace Units;

#define foreach for
#define var auto
#define null nullptr

// Not needed, but faster than replacing all C# occurances
class StringBuilder {
private:
public:
	String TheString{ "" };
	StringBuilder(String S = "") { TheString = S; }
	void Append(String S) { TheString += S; }
};

int main(int number_of_args,char **args) {
	//using System.Text;
	//using TrickyUnits;


	// This is only a quick program which will make sure a proper header file containing all APIs is present.

	try {
		QCol->Doing("Running from", CurrentDir());
		//const string APIDir = "E:/Projects/Applications/Slyvina/Apps/SCI/SCI_Run/API";
		//var APIDir{ ExtractDir(ExtractDir(args[0])) + "/SCI_Run/API" }; QCol->Doing("API dir", APIDir);
		String APIDir{ CurrentDir() };
		if (StripDir(APIDir) == "SCI_Run")
			APIDir += "/API";
		else
			APIDir = ExtractDir(APIDir) + "/SCI_Run/API";
		
		var APIS = FileList(APIDir); if (APIS == null) throw std::runtime_error("I could not get the contents of \"" + APIDir + "\"!");
		var Headers = StringBuilder("");
		var InitFunc = StringBuilder("\tinline void InstallAllAPIS() {\n");

		foreach(var APISource : *APIS) {
			if (Lower(ExtractExt(APISource)) == "cpp") {
				QCol->Doing("Adding", APISource);
				var APIName = StripAll(APISource);
				Headers.Append("\tvoid Init_" + APIName + "(); \n");
				InitFunc.Append("\t\tInit_" + APIName + "(); \n");
			} else {
				QCol->Doing("Skipping", APISource);
			}
		}


		InitFunc.Append("\t}\n");
		//const string AO = "{", AS = "}";
		String Output = (String)"namespace Scyndi_CI{\n\n"+Headers.TheString+"\n\n"+InitFunc.TheString+"}\n\n\n";
		QCol->Green(Output+"\n");
		SaveString(APIDir+"/SCI_InstallAPIs.hpp", Output);
	} catch (std::runtime_error E) {
		QCol->Error(E.what());
		//QCol->Magenta($"\n\n{E.StackTrace}\n\n");
	//} finally {
//		Console.ResetColor();
	}
	QCol->Reset();
	return 0;
}