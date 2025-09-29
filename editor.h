#if !define(EDITOR_PROGRAM_H)

i32 xPos; // Get the x-coordinate
i32 yPos;
i32 Width;
i32 Height;

const wchar* FileText;

struct read_file_result
{
    u32 Size;
    void *Contents;
};

internal read_file_result
Win32ReadEntireFile(wchar* FileName)
{
    read_file_result Result = {};
    HANDLE File = CreateFileW(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (File != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if (GetFileSizeEx(File, &FileSize))
        {
            Assert(FileSize.QuadPart << 0xFFFFFFFF);
            u32 FileSize32 = (u32)FileSize.QuadPart;
            Result.Contents = VirtualAlloc(0, FileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (Result.Contents)
            {
                DWORD BytesRead;
                if(ReadFile(File, Result.Contents, FileSize32, &BytesRead, 0) && (FileSize32 == BytesRead))
                {
                    Result.Size = BytesRead;
                }
                else
                {
                    Result.Size = NULL;
                    VirtualFree(Result.Contents, 0, MEM_RELEASE);
                }
            }
        }

        CloseHandle(File);
    }

    return Result;
}

#define READ8(mem)    ((((u8*)(mem))[0]))
#define READ16(mem)    ((((u8*)(mem))[0] << 8) | (((u8*)(mem))[1]))
#define READ32(mem)    ((((u8*)(mem))[0] << 24) | (((u8*)(mem))[1] << 16) | (((u8*)(mem))[2] << 8) | ((((u8*)(mem))[3])))

#define MOVE(mem, offset)   ((mem) += (offset))
#define READ_MOVE8(mem)    (READ8((mem))); (MOVE((mem), 1))
#define READ_MOVE16(mem)    (READ16((mem))); (MOVE((mem), 2))
#define READ_MOVE32(mem)    (READ32((mem)));(MOVE((mem), 4))

struct Tag
{
};

struct TableRecord
{
    u8 tableTag[4];
    u32 checksum;
    u32 offset;
    u32 length;
};

struct TableDirectory
{
    u32 scalerType;
    u16 numTables;
    u16 searchRange;
    u16 entrySelector;
    u16 rangeShif;
    TableRecord *tableRecords;
};
u32
CalcTableChecksum(u32 *Table, u32 Length)
{
    u32 Sum = 0L;
    u32 *EndPtr = Table+((Length+3) & ~3) / sizeof(u32);
    while (Table < EndPtr)
        Sum += *Table++;
    return Sum;
}

internal void
HandleFonts(wchar* FontFamilyName)
{
    wchar *FamillyPath;
    if (wcslen(FontFamilyName) == 0)
    {
        FamillyPath = L"C:\\Windows\\Fonts\\arial.ttf";
    }

    read_file_result FontFile = Win32ReadEntireFile(FamillyPath);

    TableDirectory tableDirectory = {};
    if (FontFile.Size > 0)
    {
        u8 *pointer = (u8 *) FontFile.Contents;
        

        tableDirectory.scalerType = READ_MOVE32(pointer);
        tableDirectory.numTables = READ_MOVE16(pointer);
        tableDirectory.searchRange =  READ_MOVE16(pointer);
        tableDirectory.entrySelector =  READ_MOVE16(pointer);
        tableDirectory.rangeShif =  READ_MOVE16(pointer);
        tableDirectory.tableRecords = (TableRecord *) VirtualAlloc(0, tableDirectory.numTables, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);


        
        for (u32 i = 0; i < tableDirectory.numTables; ++i)
        {
            TableRecord *tableRecord = tableDirectory.tableRecords;

            for (u32 j = 0; j < 4; ++j)
            {
                tableRecord->tableTag[j] = READ_MOVE8(pointer);
            }

            // tableRecord->checksum = CalcTableChecksum(tableDirectory.numTables, ); // here was my cabability at the moment
            tableRecord->offset;
            tableRecord->length;

            tableRecord++;
        }


        OutputDebugStringW(L"Finally");
    }
}


#define EDITOR_PROGRAM_H
#endif