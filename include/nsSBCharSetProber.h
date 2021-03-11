/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 * vim: et sw=2 ts=2 fdm=marker
 */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Universal charset detector code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2001
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *          Shy Shalom <shooshX@gmail.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
#ifndef nsSingleByteCharSetProber_h__
#define nsSingleByteCharSetProber_h__

#include "nsCharSetProber.h"

/** Codepoints **/

/* Illegal codepoints.*/
#define ILL 255
/* Control character. */
#define CTR 254
/* Symbols and punctuation that does not belong to words. */
#define SYM 253
/* Return/Line feeds. */
#define RET 252
/* Numbers 0-9. */
#define NUM 251

#define SB_ENOUGH_REL_THRESHOLD  1024
#define POSITIVE_SHORTCUT_THRESHOLD  (float)0.95
#define NEGATIVE_SHORTCUT_THRESHOLD  (float)0.05
#define SYMBOL_CAT_ORDER  250
#define NUMBER_OF_SEQ_CAT 4
#define POSITIVE_CAT   (NUMBER_OF_SEQ_CAT-1)
#define NEGATIVE_CAT   0

typedef struct
{
  unsigned char *charToOrderMap;    // [256] table use to find a char's order
  char *precedenceMatrix;           // [SAMPLE_SIZE][SAMPLE_SIZE]; table to find a 2-char sequence's frequency
  int freqCharCount;                // The count of frequent characters.
  float  mTypicalPositiveRatio;     // = freqSeqs / totalSeqs
  PRBool keepEnglishLetter;         // says if this script contains English characters (not implemented)
  const char* charsetName;
} SequenceModel;


class nsSingleByteCharSetProber : public nsCharSetProber{
public:
  nsSingleByteCharSetProber(SequenceModel *model)
    :mModel(model), mReversed(PR_FALSE), mNameProber(0) { Reset(); }
  nsSingleByteCharSetProber(SequenceModel *model, PRBool reversed, nsCharSetProber* nameProber)
    :mModel(model), mReversed(reversed), mNameProber(nameProber) { Reset(); }

  virtual const char* GetCharSetName();
  virtual nsProbingState HandleData(const char* aBuf, PRUint32 aLen);
  virtual nsProbingState GetState(void) {return mState;};
  virtual void      Reset(void);
  virtual float     GetConfidence(void);
  virtual void      SetOpion() {};

  // This feature is not implemented yet. any current language model
  // contain this parameter as PR_FALSE. No one is looking at this
  // parameter or calling this method.
  // Moreover, the nsSBCSGroupProber which calls the HandleData of this
  // prober has a hard-coded call to FilterWithoutEnglishLetters which gets rid
  // of the English letters.
  PRBool KeepEnglishLetters() {return mModel->keepEnglishLetter;}; // (not implemented)

#ifdef DEBUG_chardet
  virtual void  DumpStatus();
#endif

protected:
  nsProbingState mState;
  const SequenceModel *mModel;
  const PRBool mReversed; // PR_TRUE if we need to reverse every pair in the model lookup

  //char order of last character
  unsigned char mLastOrder;

  PRUint32 mTotalSeqs;
  PRUint32 mSeqCounters[NUMBER_OF_SEQ_CAT];

  PRUint32 mTotalChar;
  PRUint32 mCtrlChar;
  //characters that fall in our sampling range
  PRUint32 mFreqChar;

  // Optional auxiliary prober for name decision. created and destroyed by the GroupProber
  nsCharSetProber* mNameProber;

};

extern SequenceModel Koi8r_CyrillicModel;
extern SequenceModel Win1251_CyrillicModel;
extern SequenceModel Latin5_CyrillicModel;
extern SequenceModel MacCyrillicModel;
extern SequenceModel Ibm866_CyrillicModel;
extern SequenceModel Ibm855_CyrillicModel;
extern SequenceModel Latin7_GreekModel;
extern SequenceModel Win1253_GreekModel;
extern SequenceModel Latin5_BulgarianModel;
extern SequenceModel Win1251_BulgarianModel;
extern SequenceModel Latin2_HungarianModel;
extern SequenceModel Win1250_HungarianModel;
extern SequenceModel TIS620_ThaiModel;
extern SequenceModel Latin11_ThaiModel;
extern SequenceModel Win1255_HebrewModel;
extern SequenceModel Latin6_ArabicModel;
extern SequenceModel Windows1256_ArabicModel;
extern SequenceModel Latin1_DanishModel;
extern SequenceModel Latin15_DanishModel;
extern SequenceModel Windows1252_DanishModel;
extern SequenceModel Latin3_EsperantoModel;
extern SequenceModel Latin1_FrenchModel;
extern SequenceModel Latin15_FrenchModel;
extern SequenceModel Windows1252_FrenchModel;
extern SequenceModel Latin1_GermanModel;
extern SequenceModel Windows1252_GermanModel;
extern SequenceModel Latin1_SpanishModel;
extern SequenceModel Latin15_SpanishModel;
extern SequenceModel Windows1252_SpanishModel;
extern SequenceModel Latin3_TurkishModel;
extern SequenceModel Latin9_TurkishModel;
extern SequenceModel Viscii_VietnameseModel;
extern SequenceModel Windows1258_VietnameseModel;

#endif /* nsSingleByteCharSetProber_h__ */

