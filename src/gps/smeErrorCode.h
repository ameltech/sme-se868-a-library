/*
 * smeErrorCode.h
 *
 * Created: 5/4/2015 8:48:34 PM
 * Author : Seve (seve@axelelettronica.it)
 */


#ifndef SMEERRORCODE_H_
#define SMEERRORCODE_H_


/* Mapping error codes */
#define SME_OK           0
#define SME_EPERM        1  /* Operation not permitted */
#define SME_ERR          2
#define SME_EIO          5  /* I/O error */
#define SME_ENXIO        6  /* No such device or address */
#define SME_ENOMEM      12  /* Out of memory */
#define SME_SFX_OK      13  /* SigFox answer OK */
#define SME_SFX_KO      14  /* SigFox answer KO */

#define SME_EBUSY       16  /* Device or resource busy */
#define SME_ENODEV      19  /* No such device */
#define SME_EINVAL      22  /* Invalid argument */



#endif /* SMEERRORCODE_H_ */
