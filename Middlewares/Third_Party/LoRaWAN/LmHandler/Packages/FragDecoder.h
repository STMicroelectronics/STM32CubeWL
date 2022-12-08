/*!
 * \file      FragDecoder.h
 *
 * \brief     Implements the LoRa-Alliance fragmentation decoder
 *            Specification V1.0.0: https://resources.lora-alliance.org/technical-specifications/lorawan-fragmented-data-block-transport-specification-v1-0-0
 *            Specification V2.0.0: https://resources.lora-alliance.org/technical-specifications/ts004-2-0-0-fragmented-data-block-transport
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2018 Semtech
 *
 * \endcode
 *
 * \author    Fabien Holin ( Semtech )
 * \author    Miguel Luis ( Semtech )
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    FragDecoder.h
  * @author  MCD Application Team
  * @brief   Header for Fragmentation Decoder module
  ******************************************************************************
  */
#ifndef __FRAG_DECODER_H__
#define __FRAG_DECODER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define FRAG_SESSION_FINISHED                       ( int32_t )0
#define FRAG_SESSION_NOT_STARTED                    ( int32_t )-2
#define FRAG_SESSION_ONGOING                        ( int32_t )-1

typedef struct sFragDecoderStatus
{
    uint16_t FragNbRx;
    uint16_t FragNbLost;
    uint16_t FragNbLastRx;
    uint8_t MatrixError;
} FragDecoderStatus_t;

typedef struct sFragDecoderCallbacks
{
    /*!
     * Initialize final uncoded data buffer
     *
     * \retval status Write operation status [0: Success, -1 Fail]
     */
    int32_t ( *FragDecoderErase )( void );
    /*!
     * Writes `data` buffer of `size` starting at address `addr`
     *
     * \param [in] addr Address start index to write to.
     * \param [in] data Data buffer to be written.
     * \param [in] size Size of data buffer to be written.
     *
     * \retval status Write operation status [0: Success, -1 Fail]
     */
    int32_t ( *FragDecoderWrite )( uint32_t addr, uint8_t *data, uint32_t size );
    /*!
     * Reads `data` buffer of `size` starting at address `addr`
     *
     * \param [in] addr Address start index to read from.
     * \param [in] data Data buffer to be read.
     * \param [in] size Size of data buffer to be read.
     *
     * \retval status Read operation status [0: Success, -1 Fail]
     */
    int32_t ( *FragDecoderRead )( uint32_t addr, uint8_t *data, uint32_t size );
} FragDecoderCallbacks_t;

/*!
 * \brief Initializes the fragmentation decoder
 *
 * \param [in] fragNb     Number of expected fragments (without redundancy packets)
 * \param [in] fragSize   Size of a fragment
 * \param [in] callbacks  Pointer to the Write/Read functions.
 * \param [in] fragPVer   Fragmentation Package version to adapt the LDPC matrix usage
 */
void FragDecoderInit( uint16_t fragNb, uint8_t fragSize, FragDecoderCallbacks_t *callbacks, uint8_t fragPVer );

/*!
 * \brief Gets the maximum file size that can be received
 *
 * \retval size FileSize
 */
uint32_t FragDecoderGetMaxFileSize( void );

/*!
 * \brief Function to decode and reconstruct the binary file
 *        Called for each receive frame
 *
 * \param [in] fragCounter Fragment counter [1..(FragDecoder.FragNb + FragDecoder.Redundancy)]
 * \param [in] rawData     Pointer to the fragment to be processed (length = FragDecoder.FragSize)
 *
 * \retval status          Process status. [FRAG_SESSION_ONGOING,
 *                                          FRAG_SESSION_FINISHED or
 *                                          FragDecoder.Status.FragNbLost]
 */
int32_t FragDecoderProcess( uint16_t fragCounter, uint8_t *rawData );

/*!
 * \brief Gets the current fragmentation status
 *
 * \retval status Fragmentation decoder status
 */
FragDecoderStatus_t FragDecoderGetStatus( void );

#ifdef __cplusplus
}
#endif

#endif /* __FRAG_DECODER_H__ */
