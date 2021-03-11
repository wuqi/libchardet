libchardet - Mozilla's Universal Charset Detector C/C++ API
===
[![Build Status](https://travis-ci.org/Joungkyun/libchardet.svg?branch=master)](https://travis-ci.org/Joungkyun/libchardet)
[![GitHub license](https://img.shields.io/badge/license-MPL%201.1-blue.svg)](https://raw.githubusercontent.com/Joungkyun/libchardet/master/LICENSE)
[![GitHub last release](https://img.shields.io/github/release/joungkyun/libchardet.svg)](https://github.com/joungkyun/libchardet/releases)
[![GitHub closed issues](https://img.shields.io/github/issues-closed-raw/joungkyun/libchardet.svg)](https://github.com/joungkyun/libchardet/issues?q=is%3Aissue+is%3Aclosed)
[![GitHub closed pull requests](https://img.shields.io/github/issues-pr-closed-raw/joungkyun/libchardet.svg)](https://github.com/joungkyun/libchardet/pulls?q=is%3Apr+is%3Aclosed)

## License
Copyright (c) 2019 JoungKyun.Kim <http://oops.org> All rights reserved.

This program is under MPL 1.1 or LGPL 2.1

## Description
libchardet is based on Mozilla Universal Charset Detector library and, detects
the character set used to encode data.

[Original code](http://lxr.mozilla.org/seamonkey/source/extensions/universalchardet/) was writed by Netscape Communications Corporation, Techniques used by universalchardet are described at &lt;http://www-archive.mozilla.org/projects/intl/UniversalCharsetDetection.html&gt;.

libchardet see also John Gardiner Myers's [Encode-Detect-1.01](http://search.cpan.org/~jgmyers/Encode-Detect-1.01/)
perl module, and added C wrapping API, and library build environment with libtool.

From 1.0.5, libchardet was reflected single-byte charset detection confidence
algorithm of [uchardet](https://github.com/BYVoid/uchardet/) and new language models.
(Arabic, Danish, Esperanto, German, Spanish, Turkish, Vietnamese)

From 1.0.6, bom members have been added to the DetectObj structure.
The value of the bom member is 1, which means that it has been detected as a BOM.
Support for bom member can be determined by the existence of the CHARDET_BOM_CHECK
constant. See example below.

## Installation

See also [INSTALL](INSTALL) document

## Sample Codes

See also test directory of source code

```c
       #include <chardet.h>

       int main (void) {
            DetectObj *obj;
            char * str = "안녕하세요";

            if ( (obj = detect_obj_init ()) == NULL ) {
                 fprintf (stderr, "Memory Allocation failed\n");
                 return CHARDET_MEM_ALLOCATED_FAIL;
            }

       #ifndef CHARDET_BINARY_SAFE 
            // before 1.0.5. This API is deprecated on 1.0.5
            switch (detect (str, &obj))
       #else
            // from 1.0.5
            switch (detect_r (str, strlen (str), &obj))
       #endif
            {
                 case CHARDET_OUT_OF_MEMORY :
                      fprintf (stderr, "On handle processing, occured out of memory\n");
                      detect_obj_free (&obj);
                      return CHARDET_OUT_OF_MEMORY;
                 case CHARDET_NULL_OBJECT :
                      fprintf (stderr,
                                "2st argument of chardet() is must memory allocation "
                                "with detect_obj_init API\n");
                      return CHARDET_NULL_OBJECT;
            }

        #ifndef CHARDET_BOM_CHECK
            printf ("encoding: %s, confidence: %f\n", obj->encoding, obj->confidence);
        #else
            // from 1.0.6 support return whether exists BOM
            printf (
                "encoding: %s, confidence: %f, exist BOM: %d\n",
                 obj->encoding, obj->confidence, obj->bom
            );
        #endif
            detect_obj_free (&obj);

            return 0;
       }
```

or looping code

```c
       #include <chardet.h>

       int main (void) {
            Detect    * d;
            DetectObj * obj;
            char * str = "안녕하세요";

            if ( (d = detect_init ()) == NULL ) {
                 fprintf (stderr, "chardet handle initialize failed\n");
                 return CHARDET_MEM_ALLOCATED_FAIL;
            }

            while ( 1 ) {
                detect_reset (&d);

                if ( (obj = detect_obj_init ()) == NULL ) {
                     fprintf (stderr, "Memory Allocation failed\n");
                     return CHARDET_MEM_ALLOCATED_FAIL;
                }

       #ifndef CHARDET_BINARY_SAFE 
                // before 1.0.5. This API is deprecated on 1.0.5
                switch (detect_handledata (&d, str,, &obj))
       #else
                // from 1.0.5
                switch (detect_handledata_r (&d, str, strlen (str), &obj))
       #endif
                {
                     case CHARDET_OUT_OF_MEMORY :
                          fprintf (stderr, "On handle processing, occured out of memory\n");
                          detect_obj_free (&obj);
                          return CHARDET_OUT_OF_MEMORY;
                     case CHARDET_NULL_OBJECT :
                          fprintf (stderr,
                                    "2st argument of chardet() is must memory allocation "
                                    "with detect_obj_init API\n");
                          return CHARDET_NULL_OBJECT;
                }

        #ifndef CHARDET_BOM_CHECK
                printf ("encoding: %s, confidence: %f\n", obj->encoding, obj->confidence);
        #else
                // from 1.0.6 support return whether exists BOM
                printf (
                    "encoding: %s, confidence: %f, exist BOM: %d\n",
                    obj->encoding, obj->confidence, obj->bom
                );
        #endif
                detect_obj_free (&obj);

                if ( 1 )
                    break;
            }
            detect_destroy (&d);

           return 0;
       }
```

## APIs
  * PHP Extension    - https://github.com/OOPS-ORG-PHP/mod_chardet
  * PYTHON C Binding - https://github.com/Joungkyun/python-chardet
  * PERL             - http://search.cpan.org/~jgmyers/Encode-Detect-1.01/Detect.pm
