REM This script is ued to deploy DR controller software from tarsier.exe.
REM 1. folders to be copied into released directory:
REM     ./data: DR and detectory configurations. 
REM     ./images: tarsier APP image resources.
REM 	./work_dir: detector configurations.
REM 2. files to be copied for detector service:
REM 	2.1 detector dll:(*.dll in iDetector folder)
REM		CaliDynamic.dll CaliE4W.dll CaliE4W_V1.dll
REM		CaliE4W_V2.dll CaliImpl.dll ConnGigEVision.dll
REM		ConnSGDMA.dll ConnUDP.dll ConnUDPE50.dll
REM		ConnUdpTcp.dll ConnUdpTcpE51.dll ConnUSBfifo.dll
REM		ConnVisionPointCXP.dll CtrImagePresent.dll DbLinq.dll 
REM		DbLinq.MySql.dll DetectorProxy.dll DetFinder.dll 
REM		E3.dll E4.dll E45E.dll 
REM		E45N.dll E45W.dll E4N.dll 
REM		E4S.dll E4W.dll E4W_V1.dll 
REM		E4W_V2.dll FpdDataModel.dll FpdSys.dll 
REM		fpd_boost.dll fpd_yall.dll FTPService.dll 
REM		IDicom.dll mysql.data.dll SignalProcessing.dll 
REM		Syncbox.dll System.Data.SQLite.dll TiffParse.dll 
REM		WibuCm64.dll
REM 	2.2 detector exe:
REM 		DetectorService.exe
REM 	2.3 detector cfg files:
REM		Bind.txt components.xml
REM	2.4 license files:
REM		FpdSys.lic
REM		SignalProc.lic
REM	2.5 other unknown files:
REM		CountryChannels.db
REM		Algorithm.dll
REM 3. files and folders for qt virtual keyboard:
