namespace Scyndi_CI{

	void Init_API_Audio();
	void Init_API_Bank();
	void Init_API_BlopPlasma();
	void Init_API_DebugConsole();
	void Init_API_Events();
	void Init_API_GINIE();
	void Init_API_Graphics();
	void Init_API_InterState();
	void Init_API_JCR();
	void Init_API_Kthura();
	void Init_API_Medals();
	void Init_API_SGFile();
	void Init_API_SGJCR();
	void Init_API_Statistician();
	void Init_API_String();
	void Init_API_Vars();


	inline void InstallAllAPIS() {
		Init_API_Audio();
		Init_API_Bank();
		Init_API_BlopPlasma();
		Init_API_DebugConsole();
		Init_API_Events();
		Init_API_GINIE();
		Init_API_Graphics();
		Init_API_InterState();
		Init_API_JCR();
		Init_API_Kthura();
		Init_API_Medals();
		Init_API_SGFile();
		Init_API_SGJCR();
		Init_API_Statistician();
		Init_API_String();
		Init_API_Vars();
	}


}
