// NodeDCMTK.cpp: DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "NodeDCMTK.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

#define MAX_STRINGBUFFER_SIZE	1024

char g_string_buffer[MAX_STRINGBUFFER_SIZE] = { 0, };
void printHierarchy(DcmObject* container, DcmObject* current, unsigned int level);

int test_sum(int a, int b)
{
	return a + b;
}

void test_parameter_string(char* param)
{
	char buffer[256];
	sprintf(buffer, "test_parameter_string::param=%s", param);
	OutputDebugStringA(buffer);
}

char* test_return_string(double data)
{
	sprintf(g_string_buffer, "convert::%f", data);
	return g_string_buffer;
}

void test_loaddcm(char* fileName)
{
	DcmFileFormat dcmfileFormat;
	OFCondition status = dcmfileFormat.loadFile(fileName);
	if (!status.good())
		return;

	//DumpDcmTag(&dcmfileFormat);
	DumpDcmTagHierarchy(&dcmfileFormat);
}

void* test_get_DcmFileFormat(char* fileName)
{
	DcmFileFormat *pDcmfileFormat = new DcmFileFormat();
	OFCondition status = pDcmfileFormat->loadFile(fileName);
	if (!status.good())
		return NULL;

	return pDcmfileFormat;
}

void test_voidptr_paramter(void* dcmPtr)
{
	DcmFileFormat* pDcmFileFormat = (DcmFileFormat*)dcmPtr;
	OFString patientName;
	if (pDcmFileFormat->getDataset()->findAndGetOFString(DCM_PatientName, patientName).good())
	{
		char buffer[256];
		sprintf(buffer, "PatientName=%s", patientName);
		OutputDebugStringA(buffer);
	}
}

int OpenDcmFileFormat(char* fileName, void** dcmptr)
{
	DcmFileFormat *pDcmfileFormat = new DcmFileFormat();
	OFCondition status = pDcmfileFormat->loadFile(fileName);
	if (!status.good())
		return 0;

	*dcmptr = pDcmfileFormat;
	return 1;
}

int CloseDcmFileFormat(void* dcmPtr)
{
	DcmFileFormat* pDcmFileFormat = (DcmFileFormat*)dcmPtr;
	if (pDcmFileFormat == NULL)
		return 0;

	delete pDcmFileFormat;
	return 1;
}

int DumpDcmTag(void* dcmPtr)
{
	DcmFileFormat* pDcmFileFormat = (DcmFileFormat*)dcmPtr;
	if (pDcmFileFormat == NULL)
		return 0;

	OFCondition status;
	for (long i = 0; i < pDcmFileFormat->getDataset()->card(); i++)
	{
		DcmElement* pElement = pDcmFileFormat->getDataset()->getElement(i);
		if (pElement == NULL)
			continue;

		OFString value;
		status = pElement->getOFString(value, 0);
		if (!status.good())
			continue;

		char buffer[256];
		sprintf(buffer, "[%04X:%04X] %s : %s", pElement->getGTag(), pElement->getETag(), ((DcmTag&)pElement->getTag()).getTagName(), value);
		OutputDebugStringA(buffer);
	}
	return 1;
}

int DumpDcmTagHierarchy(void* dcmPtr)
{
	DcmFileFormat* pDcmFileFormat = (DcmFileFormat*)dcmPtr;
	if (pDcmFileFormat == NULL)
		return 0;

	DcmDataset* dset = pDcmFileFormat->getDataset();
	printHierarchy(dset, dset->nextInContainer(NULL), 0);
	return 1;
}

void printHierarchy(DcmObject* container, DcmObject* current, unsigned int level)
{
	// Check if we are already done at the current level
	if ((container == NULL) || (current == NULL)) 
		return;

	OFBool isLeaf = current->isLeaf();
	// Print current object
	char buffer[256];
	sprintf(buffer, "%s %s %s", std::string(level * 2, ' ').c_str(), current->getTag().toString().c_str(), ((DcmTag&)current->getTag()).getTagName());
	OutputDebugStringA(buffer);
	// If the current object has childs, advance to first child
	if (!current->isLeaf())
	{
		DcmStack stack;
		current->nextObject(stack, OFTrue);
		printHierarchy(current, stack.top(), level + 1);
	}
	// Advance to next object on the same level as the current one
	printHierarchy(container, container->nextInContainer(current), level);
}

int GetDcmDataSet(void* dcmPtr, void** pdataset)
{
	DcmFileFormat* pDcmFileFormat = (DcmFileFormat*)dcmPtr;
	if (pDcmFileFormat == NULL)
		return 0;

	DcmObject** dataset = (DcmObject**)pdataset;
	if (dataset == NULL)
		return 0;
	
	*dataset = pDcmFileFormat->getDataset();
	return 1;
}

int DcmObjectNextInContainer(void* pcontainer, void* pcurrent, void** pnextObject)
{
	DcmObject* container = (DcmObject*)pcontainer;
	DcmObject* current = (DcmObject*)pcurrent;
	DcmObject** nextObject = (DcmObject**)pnextObject;
	if (container == NULL || nextObject == NULL)
		return 0;

	*nextObject = container->nextInContainer(current);
	return 1;
}
    
int DcmObjectNextObjectTop(void* pcurrent, void** pnextObject)
{
	DcmObject* current = (DcmObject*)pcurrent;
	DcmObject** nextObject = (DcmObject**)pnextObject;
	if (current == NULL || nextObject == NULL)
		return 0;

	DcmStack stack;
	current->nextObject(stack, OFTrue);
	*nextObject = stack.top();
	return 1;
}

int GetElementCount(void* dcmPtr, long* count)
{
	DcmFileFormat* pDcmFileFormat = (DcmFileFormat*)dcmPtr;
	if (pDcmFileFormat == NULL)
		return 0;

	*count = pDcmFileFormat->getDataset()->card();
	return 1;
}

int GetElement(void* dcmPtr, int index, void** elementPtr)
{
	DcmFileFormat* pDcmFileFormat = (DcmFileFormat*)dcmPtr;
	if (pDcmFileFormat == NULL)
		return 0;

	*elementPtr = pDcmFileFormat->getDataset()->getElement(index);
	return 1;
}

int GetElementGTag(void* elementPtr, unsigned short* gtag)
{
	DcmElement* pElement = (DcmElement*)elementPtr;
	if (pElement == NULL)
		return 0;

	*gtag = pElement->getGTag();
	return 1;
}

int GetElementETag(void* elementPtr, unsigned short* etag)
{
	DcmElement* pElement = (DcmElement*)elementPtr;
	if (pElement == NULL)
		return 0;

	*etag = pElement->getETag();
	return 1;
}

int GetElementTagName(void* elementPtr, char* tagName)
{
	DcmElement* pElement = (DcmElement*)elementPtr;
	if (pElement == NULL)
		return 0;

	char* name = (char*)((DcmTag&)pElement->getTag()).getTagName();
	strcpy(tagName, name);
	return 1;
}

int GetElementStringValue(void* elementPtr, char* value)
{
	DcmElement* pElement = (DcmElement*)elementPtr;
	if (pElement == NULL)
		return 0;

	if (pElement->isEmpty() || pElement->isLeaf() == false)
		return 0;

	OFString ofValue;
	try 
	{
		OFCondition status = pElement->getOFString(ofValue, 0);
		if (!status.good())
			return 0;
	}
	catch (...)
	{
		OutputDebugStringA("GetElementStringValue Exception!");
		return 0;
	}

	strcpy(value, ofValue.c_str());
	return 1;
}

int GetElementVR(void* elementPtr, char* vr)
{
	DcmElement* pElement = (DcmElement*)elementPtr;
	if (pElement == NULL)
		return 0;

	DcmVR dcmVR(pElement->getVR());
	strcpy(vr, dcmVR.getVRName());
	return 1;
}

int IsLeafElement(void* elementPtr, unsigned char* isLeaf)
{
	DcmElement* pElement = (DcmElement*)elementPtr;
	if (pElement == NULL)
		return 0;

	*isLeaf = pElement->isLeaf() == true ? 1 : 0;
	return 1;
}

