// copyright (c) 2017-2021 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "xsevenzip.h"

XSevenZip::XSevenZip(QIODevice *pDevice) : XArchive(pDevice)
{

}

bool XSevenZip::isValid()
{
    bool bResult=false;

    if(getSize()>(qint64)sizeof(SignatureHeader))
    {
        if(compareSignature("'7z'BCAF271C"))
        {
            bResult=true;
        }
    }

    return bResult;
}

bool XSevenZip::isValid(QIODevice *pDevice)
{
    XSevenZip xsevenzip(pDevice);

    return xsevenzip.isValid();
}

QString XSevenZip::getVersion()
{
    return QString("%1.%2").arg(read_uint8(6)).arg(read_uint8(7),1,10,QChar('0'));
}

quint64 XSevenZip::getNumberOfRecords()
{
    quint64 nResult=0;

    SignatureHeader signatureHeader;

    if(read_array(0,(char *)&signatureHeader,sizeof(SignatureHeader))==sizeof(SignatureHeader))
    {
        _MEMORY_MAP memoryMap=XBinary::getMemoryMap();

        qint64 nCurrentOffset=sizeof(SignatureHeader)+signatureHeader.NextHeaderOffset;

        if(isOffsetAndSizeValid(&memoryMap,nCurrentOffset,signatureHeader.NextHeaderSize)) // TODO Handle errors!
        {
            quint64 nCurrentSize=0;

            while(nCurrentSize<signatureHeader.NextHeaderSize)
            {
                PACKED pn=get_packedNumber(nCurrentOffset);

                if(pn.nValue==k7zIdPackInfo)
                {
                    nCurrentOffset+=pn.nByteSize;
                    nCurrentSize+=pn.nByteSize;
                    pn=get_packedNumber(nCurrentOffset); // Offset
                    nCurrentOffset+=pn.nByteSize;
                    nCurrentSize+=pn.nByteSize;
                    pn=get_packedNumber(nCurrentOffset); // Number of Streams

                    nResult=pn.nValue;

                    break;
                }

//                QString sDebugString=QString("%1 %2").arg(uleb.nValue).arg(uleb.nByteSize);
//                qDebug("%s",sDebugString.toLatin1().data());
                nCurrentOffset+=pn.nByteSize;
                nCurrentSize+=pn.nByteSize;
            }

            // TODO Encrypted
        }
    }

    return nResult;
}

QList<XArchive::RECORD> XSevenZip::getRecords(qint32 nLimit)
{
    Q_UNUSED(nLimit)

    QList<XArchive::RECORD> listResult;

    return listResult;

    // TODO

    SignatureHeader signatureHeader={};

    if(read_array(0,(char *)&signatureHeader,sizeof(SignatureHeader))==sizeof(SignatureHeader)) // TODO read function
    {
        _MEMORY_MAP memoryMap=XBinary::getMemoryMap();

        qint64 nCurrentOffset=sizeof(SignatureHeader)+signatureHeader.NextHeaderOffset;

        if(isOffsetAndSizeValid(&memoryMap,nCurrentOffset,signatureHeader.NextHeaderSize)) // TODO Handle errors!
        {
            // Header
            // k7zIdHeader
            // k7zIdMainStreamsInfo
            //  k7zIdPackInfo
            //      dataOffset
            //      numberOfFiles;
            //      Sizes[numberOfFiles]
            //  k7zIdUnpackInfo
            //  k7zIdEnd
            //  k7zIdUnpackInfo
            //      k7zIdFolder
            //      numberOfFiles
            //      0
            //      NumCoders[numberOfFiles]
            //      MainByte[numberOfFiles]
            //      Methods[numberOfFiles]
            //      k7zIdCodersUnpackSize
            //      numberOfFiles
            //      Sizes[numberOfFiles]

            PACKED pn=get_packedNumber(nCurrentOffset);

            if(pn.nValue==k7zIdHeader)
            {
                XHEADER header={};

                nCurrentOffset+=getHeader(nCurrentOffset,&header);
            }
            else if(pn.nValue==k7zIdEncodedHeader)
            {
                XHEADER header={};

                nCurrentOffset+=getEncodedHeader(nCurrentOffset,&header);
            }

            while(true)
            {
                PACKED pn=get_packedNumber(nCurrentOffset);

                if(pn.nValue==k7zIdHeader)
                {
                    qDebug("k7zIdHeader");
                }
                else if(pn.nValue==k7zIdMainStreamsInfo)
                {
                    qDebug("k7zIdMainStreamsInfo");
                }
                else if(pn.nValue==k7zIdPackInfo)
                {
                    qDebug("k7zIdPackInfo");
                }
                else if(pn.nValue==k7zIdUnpackInfo)
                {
                    qDebug("k7zIdUnpackInfo");
                }
                else if(pn.nValue==k7zIdFolder)
                {
                    qDebug("k7zIdFolder");
                }
                else if(pn.nValue==k7zIdCodersUnpackSize)
                {
                    qDebug("k7zIdCodersUnpackSize");
                }
                else if(pn.nValue==k7zIdEncodedHeader)
                {
                    qDebug("k7zIdEncodedHeader");
                }

                qDebug("Test %d",pn.nValue);

                nCurrentOffset+=pn.nByteSize;
            }

            XHEADER header={};

            getEncodedHeader(nCurrentOffset,&header);

            quint64 nCurrentSize=0;

            qint64 nDataOffset=0;

            while(nCurrentSize<signatureHeader.NextHeaderSize)
            {
                PACKED pn=get_packedNumber(nCurrentOffset);

//                if(pn.nValue==k7zIdPackInfo)
//                {
//                    nCurrentOffset+=pn.nByteSize;
//                    nCurrentSize+=pn.nByteSize;
//                    pn=get_packedNumber(nCurrentOffset); // Offset
//                    nDataOffset=pn.nValue;
//                    nCurrentSize+=pn.nByteSize;
//                    pn=get_packedNumber(nCurrentOffset); // Number of Streams
//                    nNumberOfFiles=pn.nValue;

//                    for(qint64 i=0;i<nNumberOfFiles;i++)
//                    {
//                        RECORD record={};
//                        listResult.append(record);
//                    }
//                }

                QString sDebugString=QString("%1 %2 %3").arg(pn.nValue).arg(pn.nByteSize).arg(idToSring((EIdEnum)pn.nValue));
                qDebug("%s",sDebugString.toLatin1().data());
                nCurrentOffset+=pn.nByteSize;
                nCurrentSize+=pn.nByteSize;

                if(pn.nValue==k7zIdEncodedHeader)
                {

                }
                if(pn.nValue==k7zIdPackInfo)
                {
                    pn=get_packedNumber(nCurrentOffset);
                    qDebug("Current Offset: %llu",pn.nValue);
                    nDataOffset=pn.nValue;
                    nCurrentOffset+=pn.nByteSize;
                    nCurrentSize+=pn.nByteSize;
                    pn=get_packedNumber(nCurrentOffset);
                    qint64 nNumberOfStreams=pn.nValue;
                    qDebug("Number Of Streams: %lld",nNumberOfStreams);
                    nCurrentOffset+=pn.nByteSize;
                    nCurrentSize+=pn.nByteSize;

                    pn=get_packedNumber(nCurrentOffset);
                    nCurrentOffset+=pn.nByteSize;
                    nCurrentSize+=pn.nByteSize;

                    if(pn.nValue==k7zIdSize)
                    {
                        qDebug("Size");

                        for(int i=0;i<nNumberOfStreams;i++)
                        {
                            pn=get_packedNumber(nCurrentOffset);
                            qDebug("Current Size: %llu",pn.nValue);
                            nDataOffset=pn.nValue;
                            nCurrentOffset+=pn.nByteSize;
                            nCurrentSize+=pn.nByteSize;
                        }
                    }

                    pn=get_packedNumber(nCurrentOffset);

                    if(pn.nValue==k7zIdEnd)
                    {
                        qDebug("End");
                    }

                    nCurrentOffset+=pn.nByteSize;
                    nCurrentSize+=pn.nByteSize;
                }
                if(pn.nValue==k7zIdUnpackInfo)
                {
                    pn=get_packedNumber(nCurrentOffset);
                    nCurrentOffset+=pn.nByteSize;
                    nCurrentSize+=pn.nByteSize;

                    if(pn.nValue==k7zIdFolder)
                    {
                        pn=get_packedNumber(nCurrentOffset);
                        qint64 nNumberOfFolders=pn.nValue;
                        qDebug("Number of folders: %llu",pn.nValue);
                        nCurrentOffset+=pn.nByteSize;
                        nCurrentSize+=pn.nByteSize;

                        quint8 nExtra=read_uint8(nCurrentOffset);
                        // TODO compare to 0
                        nCurrentOffset++;
                        nCurrentSize++;

                        for(int i=0;i<nNumberOfFolders;i++)
                        {
                            pn=get_packedNumber(nCurrentOffset);
                            qint64 nNumberOfCoders=pn.nValue;
                            qDebug("Number of coders: %lld",nNumberOfCoders);
                            // TODO
                            nCurrentOffset+=pn.nByteSize;
                            nCurrentSize+=pn.nByteSize;

                            for(int j=0;j<nNumberOfCoders;j++)
                            {
                                quint8 nMainByte=read_uint8(nCurrentOffset);

                                nCurrentOffset++;
                                nCurrentSize++;

                                if((nMainByte&0xC0)==0)
                                {
                                    qint32 nCoderSize=nMainByte&0x0F;

                                    if((nCoderSize<=8)&&(nCoderSize<getSize()))
                                    {
                                        nCurrentOffset+=nCoderSize;
                                        nCurrentSize+=nCoderSize;
                                    }
                                    else
                                    {
                                        qDebug("Error");
                                    }
                                }
                                else
                                {
                                    qDebug("Error");
                                }
                                // TODO compare to 0
                                // TODO Check !!!
                            }
                        }
                    }
                }
            }
            // TODO Encrypted
        }
    }

    return listResult;
}

QString XSevenZip::idToSring(XSevenZip::EIdEnum id)
{
    QString sResult="Unknown";

    switch(id)
    {
        case k7zIdEnd:                      sResult=QString("k7zIdEnd");                        break;
        case k7zIdHeader:                   sResult=QString("k7zIdHeader");                     break;
        case k7zIdArchiveProperties:        sResult=QString("k7zIdArchiveProperties");          break;
        case k7zIdAdditionalStreamsInfo:    sResult=QString("k7zIdAdditionalStreamsInfo");      break;
        case k7zIdMainStreamsInfo:          sResult=QString("k7zIdMainStreamsInfo");            break;
        case k7zIdFilesInfo:                sResult=QString("k7zIdFilesInfo");                  break;
        case k7zIdPackInfo:                 sResult=QString("k7zIdPackInfo");                   break;
        case k7zIdUnpackInfo:               sResult=QString("k7zIdUnpackInfo");                 break;
        case k7zIdSubStreamsInfo:           sResult=QString("k7zIdSubStreamsInfo");             break;
        case k7zIdSize:                     sResult=QString("k7zIdSize");                       break;
        case k7zIdCRC:                      sResult=QString("k7zIdCRC");                        break;
        case k7zIdFolder:                   sResult=QString("k7zIdFolder");                     break;
        case k7zIdCodersUnpackSize:         sResult=QString("k7zIdCodersUnpackSize");           break;
        case k7zIdNumUnpackStream:          sResult=QString("k7zIdNumUnpackStream");            break;
        case k7zIdEmptyStream:              sResult=QString("k7zIdEmptyStream");                break;
        case k7zIdEmptyFile:                sResult=QString("k7zIdEmptyFile");                  break;
        case k7zIdAnti:                     sResult=QString("k7zIdAnti");                       break;
        case k7zIdName:                     sResult=QString("k7zIdName");                       break;
        case k7zIdCTime:                    sResult=QString("k7zIdCTime");                      break;
        case k7zIdATime:                    sResult=QString("k7zIdATime");                      break;
        case k7zIdMTime:                    sResult=QString("k7zIdMTime");                      break;
        case k7zIdWinAttrib:                sResult=QString("k7zIdWinAttrib");                  break;
        case k7zIdComment:                  sResult=QString("k7zIdComment");                    break;
        case k7zIdEncodedHeader:            sResult=QString("k7zIdEncodedHeader");              break;
        case k7zIdStartPos:                 sResult=QString("k7zIdStartPos");                   break;
        case k7zIdDummy:                    sResult=QString("k7zIdDummy");                      break;
    }

    return sResult;
}

qint64 XSevenZip::getHeader(qint64 nOffset, XSevenZip::XHEADER *pHeader)
{
    qint64 nCurrent=0;

    PACKED pn={};

    pn=get_packedNumber(nOffset+nCurrent);

    if(pn.nValue==k7zIdHeader)
    {
        nCurrent+=pn.nByteSize;
        pHeader->bIsValid=true;

        nCurrent+=getPackInfo(nOffset+nCurrent,&(pHeader->xPackInfo));
        nCurrent+=getUnpackInfo(nOffset+nCurrent,&(pHeader->xUnpackInfo));
    }

    return nCurrent;
}

qint64 XSevenZip::getEncodedHeader(qint64 nOffset, XSevenZip::XHEADER *pHeader)
{
    qint64 nCurrent=0;

    PACKED pn={};

    pn=get_packedNumber(nOffset+nCurrent);

    if(pn.nValue==k7zIdEncodedHeader)
    {
        nCurrent+=pn.nByteSize;
        pHeader->bIsValid=true;

        nCurrent+=getPackInfo(nOffset+nCurrent,&(pHeader->xPackInfo));
        nCurrent+=getUnpackInfo(nOffset+nCurrent,&(pHeader->xUnpackInfo));
    }

    return nCurrent;
}

qint64 XSevenZip::getPackInfo(qint64 nOffset, XSevenZip::XPACKINFO *pPackInfo)
{
    qint64 nCurrent=0;

    PACKED pn={};
    pn=get_packedNumber(nOffset+nCurrent);

    if(pn.nValue==k7zIdPackInfo)
    {
        nCurrent+=pn.nByteSize;
        pn=get_packedNumber(nOffset+nCurrent);
        pPackInfo->nDataOffset=pn.nValue;
        nCurrent+=pn.nByteSize;
        pn=get_packedNumber(nOffset+nCurrent);

        quint64 nSize=pn.nValue;
        nCurrent+=pn.nByteSize;

        pn=get_packedNumber(nOffset+nCurrent);

        if(pn.nValue==k7zIdSize)
        {
            nCurrent+=pn.nByteSize;

            for(int i=0;i<nSize;i++)
            {
                pn=get_packedNumber(nOffset+nCurrent);

                pPackInfo->listSizes.append(pn.nValue);
                nCurrent+=pn.nByteSize;
            }
            pn=get_packedNumber(nOffset+nCurrent);

            if(pn.nValue==k7zIdEnd)
            {
                pPackInfo->bIsValid=true;
            }

            nCurrent+=pn.nByteSize;
        }
    }

    return nCurrent;
}

qint64 XSevenZip::getUnpackInfo(qint64 nOffset, XSevenZip::XUNPACKINFO *pUnpackInfo)
{
    qint64 nCurrent=0;

    PACKED pn={};
    pn=get_packedNumber(nOffset+nCurrent);

    if(pn.nValue==k7zIdUnpackInfo)
    {
        nCurrent+=pn.nByteSize;
        pn=get_packedNumber(nOffset+nCurrent);

        if(pn.nValue==k7zIdFolder)
        {
            nCurrent+=pn.nByteSize;
            pn=get_packedNumber(nOffset+nCurrent);

            quint64 nFolderCount=pn.nValue;
            nCurrent+=pn.nByteSize;

            pn=get_packedNumber(nOffset+nCurrent);

            if(pn.nValue==0) // Extra
            {
                nCurrent+=pn.nByteSize;

                // TODO Check!
                for(int i=0;i<nFolderCount;i++)
                {
                    pn=get_packedNumber(nOffset+nCurrent);

                    quint64 nNumberOfCoders=pn.nValue;
                    nCurrent+=pn.nByteSize;

                    for(int j=0;j<nNumberOfCoders;j++)
                    {
                        // TODO read coder function
                        pn=get_packedNumber(nOffset+nCurrent);
                        nCurrent+=pn.nByteSize;
                    }
                }

                pn=get_packedNumber(nOffset+nCurrent);

                // TODO Check k7zIdAdditionalStreamsInfo

                if(pn.nValue==k7zIdCodersUnpackSize)
                {
                    nCurrent+=pn.nByteSize;

                    for(int i=0;i<nFolderCount;i++)
                    {
                        pn=get_packedNumber(nOffset+nCurrent);

                        pUnpackInfo->listCoderSizes.append(pn.nValue);
                        nCurrent+=pn.nByteSize;
                    }
                }

                pn=get_packedNumber(nOffset+nCurrent);

                if(pn.nValue==k7zIdEnd)
                {
                    pUnpackInfo->bIsValid=true;
                }

                nCurrent+=pn.nByteSize;
            }
        }
    }

    return nCurrent;
}
