/** \file XFrameConvert.h */
#ifndef _XENICS_XFRAMECONVERTER_H_
#   define _XENICS_XFRAMECONVERTER_H_

/**/////////////////////////////////////////////////////////////////////////////
//// XenicsAPI - Interface definition
////////////////////////////////////////////////////////////////////////////////

#include <XCamera.h>

#ifdef __cplusplus
    extern "C" {
#endif


/** Basic types */
typedef int            XFCHANDLE;                /**< handle to XFrameConvert     */

/// This is the list of possible frame convertions
typedef enum  
{
    XFC_Correction  =  0   //< A raw image is corrected applying the data from a xenics correction (.xca) file.
} XFrameConversionType;

#ifdef __cplusplus
    }
#endif



#ifdef __cplusplus
    class XFrameConvert
    {
    protected:
        XFrameConvert(){};

    public:
        /// Creates a file converter. 
        /// IMPORTANT: only a single XFrameConvert instance can exist simultanuously. 
        /// Before you can create an instance with other parameters, delete the current instance.
        /// @param width             The width of the frames to be converted
        /// @param height            The height of the frames to be converted
        /// @param pixelSize         The bit depth of the pixels composing the frames
        /// @param conversionType    @sa XFrameConvertionType
        /// @return XFrameConvert    Returns a pointer to a XFrameConvert instance.
        static XFrameConvert IMPEXP *Create(int width, int height, int pixelSize, XFrameConversionType conversionType);
        
        virtual~XFrameConvert(){};
        
        ///Converts the source frame 
        /// @param SrcFrame        Pointer to the source frame
        /// @param SrcFrameSize    The size of the source frame in bytes: width x heigth x pixelSize + footerSize
        /// @param DstFrame        Pointer to a buffer. When this pointer is non zero, the source frame will be unaffected while the converted frame is copied into the this buffer.
        /// @param DstFrameSize    Size of the buffer
        virtual ErrCode Convert(void* SrcFrame, int SrcFrameSize, void* DstFrame = (void*)0 , int DstFrameSize = 0 )   = 0;

        /// Sets the value of a named property
        /// @param pPrp     The property name (for example: "IntegrationTime")
        /// @param pValue   The value to set the property to
        /// @param pUnit    RFU
        /// @return ErrCode @sa ErrorCodes
        virtual ErrCode     SetPropertyValue    (const char *pPrp, const char *pValue       , const char *pUnit = ((const char*)0))=0;
        virtual ErrCode     SetPropertyValueL   (const char *pPrp, long lValue              , const char *pUnit = ((const char*)0))=0;  ///< Available for easy access
        virtual ErrCode     SetPropertyValueF   (const char *pPrp, double dValue            , const char *pUnit = ((const char*)0))=0;  ///< Available for easy access
        
        /// Fetches the value of a named property
        /// For non camera readable properties this retreives the value last set, or the default.
        /// @param pPrp     The property name (for example: "IntegrationTime")
        /// @param pValue   A pointer that will receive the value.
        /// @param iMaxLen  The number of bytes reserved in the destination pointer pValue
        /// @return ErrCode @sa ErrorCodes
        virtual ErrCode     GetPropertyValue    (const char *pPrp, char *pValue     , int iMaxLen) = 0;
        virtual ErrCode     GetPropertyValueL   (const char *pPrp, long *pValue     ) = 0;                                                ///< Available for easy access
        virtual ErrCode     GetPropertyValueF   (const char *pPrp, double *pValue   ) = 0;                                                ///< Available for easy access
        
        /// Loads the file that contains the correction data.
        /// @param p_cFileName The full path to the calibration data. For instance "C:\\Temp\\ConvertFile\\AutoExposure_2086.xca"
        /// @return ErrCode @sa ErrorCodes
        virtual ErrCode     LoadCalibration     (const char *p_cFileName )= 0;
    };
    

#endif //__cplusplus

#ifdef __cplusplus
    extern "C" {
#endif

    XFCHANDLE    IMPEXPC XFC_OpenFrameConvert(int width, int height, int pixelSize, XFrameConversionType type);                                  ///< @sa XFrameConvertor::Create
    void         IMPEXPC XFC_CloseFrameConvert(XFCHANDLE h);                                                                                     ///< @sa XFrameConvertor::~XFrameConvertor
    ErrCode      IMPEXPC XFC_Convert(XFCHANDLE h, void* pSrcFrame, int iSrcFrameSize, void* pDstFrame , int iDstFrameSize );
    
    ErrCode     IMPEXPC XFC_SetPropertyValue             (XFCHANDLE h,const char *pPrp, const char *pValue        , const char *pUnit);           ///< @sa XFrameConvertor::SetPropertyValue              
    ErrCode     IMPEXPC XFC_SetPropertyValueL            (XFCHANDLE h,const char *pPrp, long lValue               , const char *pUnit);           ///< @sa XFrameConvertor::SetPropertyValueL         
    ErrCode     IMPEXPC XFC_SetPropertyValueF            (XFCHANDLE h,const char *pPrp, double dValue             , const char *pUnit);           ///< @sa XFrameConvertor::SetPropertyValueF         

    ErrCode     IMPEXPC XFC_GetPropertyValue             (XFCHANDLE h,const char *pPrp, char *pValue              , int iMaxLen);                 ///< @sa XFrameConvertor::GetPropertyValue              
    ErrCode     IMPEXPC XFC_GetPropertyValueL            (XFCHANDLE h,const char *pPrp, long *pValue      );                                      ///< @sa XFrameConvertor::GetPropertyValueL         
    ErrCode     IMPEXPC XFC_GetPropertyValueF            (XFCHANDLE h,const char *pPrp, double *pValue    );                                      ///< @sa XFrameConvertor::GetPropertyValueF

    ErrCode     IMPEXPC XFC_LoadCalibration              (XCHANDLE h, const char *p_cFileName);                                                   ///< @sa XFrameConvertor::LoadCalibration




#ifdef __cplusplus
    }
#endif

#endif //_XENICS_XFRAMECONVERTER_H_
