namespace Scyndi_CI{

	void Init_API_Events();
	void Init_API_Graphics();
	void Init_API_Vars();


	inline void InstallAllAPIS() {
		Init_API_Events();
		Init_API_Graphics();
		Init_API_Vars();
	}


}
