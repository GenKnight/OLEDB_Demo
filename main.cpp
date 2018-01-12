#include <tchar.h>
#include <windows.h>
#include <strsafe.h>

#define COM_NO_WINDOWS_H    //����Ѿ�������Windows.h��ʹ������Windows�⺯��ʱ
#define OLEDBVER 0x0260     //MSDAC2.6��
#include <oledb.h>
#include <oledberr.h>

#define GRS_ALLOC(sz)		HeapAlloc(GetProcessHeap(),0,sz)
#define GRS_CALLOC(sz)		HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sz)
#define GRS_SAFEFREE(p)		if(NULL != p){HeapFree(GetProcessHeap(),0,p);p=NULL;}

#define GRS_USEPRINTF() TCHAR pBuf[1024] = {}
#define GRS_PRINTF(...) \
	GRS_USEPRINTF();\
	StringCchPrintf(pBuf,1024,__VA_ARGS__);\
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),pBuf,lstrlen(pBuf),NULL,NULL);

#define GRS_SAFERELEASE(I)\
	if(NULL != (I))\
	{\
		(I)->Release();\
		(I)=NULL;\
	}

#define GRS_COM_CHECK(hr,...)\
	if(FAILED(hr))\
	{\
		GRS_PRINTF(__VA_ARGS__);\
		goto CLEAR_UP;\
	}

int _tmain(int argc, TCHAR* argv[])
{
	CoInitialize(NULL);
	//����OLEDB init�ӿ�
	IDBInitialize *pDBInit = NULL;
	IDBProperties *pIDBProperties = NULL;
	//������������
	DBPROPSET dbPropset[1] = {0};
	DBPROP dbProps[5] = {0};
	CLSID clsid_MSDASQL = {0}; //sql server ������Դ����
	
	HRESULT hRes = CLSIDFromProgID(_T("SQLOLEDB"), &clsid_MSDASQL);
	GRS_COM_CHECK(hRes, _T("��ȡSQLOLEDB��CLSIDʧ�ܣ������룺0x%08x\n"), hRes);
	hRes = CoCreateInstance(clsid_MSDASQL, NULL, CLSCTX_INPROC_SERVER, IID_IDBInitialize,(void**)&pDBInit);
	GRS_COM_CHECK(hRes, _T("�޷�����IDBInitialize�ӿڣ������룺0x%08x\n"), hRes);

	//ָ�����ݿ�ʵ����������ʹ���˱���local��ָ������Ĭ��ʵ��
	dbProps[0].dwPropertyID = DBPROP_INIT_DATASOURCE;
	dbProps[0].dwOptions = DBPROPOPTIONS_REQUIRED;
	dbProps[0].vValue.vt = VT_BSTR;
	dbProps[0].vValue.bstrVal = SysAllocString(OLESTR("LIU-PC\\SQLEXPRESS"));
	dbProps[0].colid = DB_NULLID;

	//ָ�����ݿ����
	dbProps[1].dwPropertyID = DBPROP_INIT_CATALOG;
	dbProps[1].dwOptions = DBPROPOPTIONS_REQUIRED;
	dbProps[1].vValue.vt = VT_BSTR;
	dbProps[1].vValue.bstrVal = SysAllocString(OLESTR("Study"));
	dbProps[1].colid = DB_NULLID;

	//ָ���������ݿ���û���
	dbProps[2].dwPropertyID = DBPROP_AUTH_USERID;
	dbProps[2].vValue.vt = VT_BSTR;
	dbProps[2].vValue.bstrVal = SysAllocString(OLESTR("sa"));
	
	//ָ���������ݿ���û�����
	dbProps[3].dwPropertyID = DBPROP_AUTH_PASSWORD;
	dbProps[3].vValue.vt = VT_BSTR;
	dbProps[3].vValue.bstrVal = SysAllocString(OLESTR("123456"));
	
	
	//��������
	hRes = pDBInit->QueryInterface(IID_IDBProperties, (void**)&pIDBProperties);
	GRS_COM_CHECK(hRes, _T("��ѯIDBProperties�ӿ�ʧ��, ������:%08x\n"), hRes);
	dbPropset->guidPropertySet = DBPROPSET_DBINIT;
	dbPropset[0].cProperties = 4;
	dbPropset[0].rgProperties = dbProps;
	hRes = pIDBProperties->SetProperties(1, dbPropset);
	GRS_COM_CHECK(hRes, _T("��������ʧ��, ������:%08x\n"), hRes);

	//�������ݿ�
	hRes = pDBInit->Initialize();
	GRS_COM_CHECK(hRes, _T("�������ݿ�ʧ�ܣ�������:%08x\n"), hRes);
	//do something
	pDBInit->Uninitialize();

	GRS_PRINTF(_T("���ݿ�����ɹ�!!!!!\n"));
CLEAR_UP:
	GRS_SAFEFREE(pDBInit);
	GRS_SAFEFREE(pIDBProperties);
	CoUninitialize();
	return 0;
}
