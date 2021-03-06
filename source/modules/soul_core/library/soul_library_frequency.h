/*
    _____ _____ _____ __
   |   __|     |  |  |  |      The SOUL language
   |__   |  |  |  |  |  |__    Copyright (c) 2019 - ROLI Ltd.
   |_____|_____|_____|_____|

   The code in this file is provided under the terms of the ISC license:

   Permission to use, copy, modify, and/or distribute this software for any purpose
   with or without fee is hereby granted, provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

R"library(

/** Discrete Fourier Transform functions. */
namespace soul::DFT
{
    /** Performs a real forward DFT from an input buffer to an output buffer. */
    void forward<SampleBuffer> (const SampleBuffer& inputData, SampleBuffer& outputData)
    {
        static_assert (SampleBuffer.isFixedSizeArray || SampleBuffer.isVector, "The buffers for DFT::forward() must be fixed size arrays");
        static_assert (SampleBuffer.elementType.isFloat && SampleBuffer.elementType.isPrimitive, "The element type for DFT::forward() must be floating point");
        let harmonics = inputData.size / 2;

        SampleBuffer inputImag, outputReal, outputImag;

        performComplex (inputData, inputImag, outputReal, outputImag, 1.0f / float (harmonics));

        outputData[0:harmonics]             = outputReal[0:harmonics];
        outputData[harmonics:harmonics * 2] = outputImag[0:harmonics];
    }

    /** Performs a real inverse DFT from an input buffer to an output buffer. */
    void inverse<SampleBuffer> (const SampleBuffer& inputData, SampleBuffer& outputData)
    {
        static_assert (SampleBuffer.isFixedSizeArray || SampleBuffer.isVector, "The buffers for DFT::inverse() must be fixed size arrays");
        static_assert (SampleBuffer.elementType.isFloat && SampleBuffer.elementType.isPrimitive, "The element type for DFT::inverse() must be floating point");
        let harmonics = inputData.size / 2;

        SampleBuffer inputReal, inputImag, outputReal;

        inputReal[0:harmonics] = inputData[harmonics:harmonics * 2];
        inputImag[0:harmonics] = inputData[0:harmonics];

        performComplex (inputReal, inputImag, outputReal, outputData, 1.0f);
    }

    /** For internal use by the other functions: performs a O(N^2) complex DFT. */
    void performComplex<SampleBuffer> (const SampleBuffer& inputReal,
                                       const SampleBuffer& inputImag,
                                       SampleBuffer& outputReal,
                                       SampleBuffer& outputImag,
                                       SampleBuffer.elementType scaleFactor)
    {
        let size = SampleBuffer.size;

        for (int i = 0; i < size; ++i)
        {
            float64 sumReal, sumImag;

            for (int j = 0; j < size; ++j)
            {
                let angle = SampleBuffer.elementType (twoPi * j * i / size);
                let sinAngle = sin (angle);
                let cosAngle = cos (angle);

                sumReal += inputImag.at(j) * cosAngle + inputReal.at(j) * sinAngle;
                sumImag += inputImag.at(j) * sinAngle - inputReal.at(j) * cosAngle;
            }

            outputImag.at(i) = SampleBuffer.elementType (sumImag) * scaleFactor;
            outputReal.at(i) = SampleBuffer.elementType (sumReal) * scaleFactor;
        }
    }
}

)library"
