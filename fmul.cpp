#include <cstdio>
#include <cstdint>


#define A -0.01
#define B 0.03

// const float A = 4.3;
// const float B = 1.1;

struct Float32{
	uint32_t mantissa: 23;
	uint32_t exponent: 8;
	uint32_t msign: 1;
};

const uint16_t MAX_EXPONENT = 0xFF;
const uint16_t BIAS = 0x7F;
const Float32 FLOAT_INFINITY { .mantissa=0, .exponent=0xff, .msign=0 };

float fmul(float af, float bf) {
	Float32 a = *(Float32*) &af;
	Float32 b = *(Float32*) &bf;
	
	uint64_t mantissa = (uint64_t(a.mantissa|(1<<23)) * uint64_t(b.mantissa|(1<<23))) >> 23;
	uint16_t biased_exponent = a.exponent + b.exponent - BIAS;
	uint16_t sign = a.msign ^ b.msign;

	// mantissa normalization
	while (mantissa >= (0x1<<24)) {
		mantissa >>= 1;
		biased_exponent += 1;
	}

	// saturating if exponent too high
	if (biased_exponent > MAX_EXPONENT) {
		Float32 inf = FLOAT_INFINITY;
		inf.msign = sign;
		return *(float*) &inf;
	}

	Float32 result;
	result.mantissa = mantissa;
	result.exponent = biased_exponent;
	result.msign = sign;
	
	return *(float*) &result;
}

int main() {
	printf("Input: a=%f, b=%f \n", A, B);

	float result = fmul(A, B);
	float expected_sprec = float(A)*float(B);
	float expected_dprec = double(A)*double(B);
	Float32 r1 = *(Float32*) &result;
	Float32 r2 = *(Float32*) &expected_sprec;
	Float32 r3 = *(Float32*) &expected_dprec;

	printf("Emulated single precision computation: %.12f Mantissa: %06x Exponent: %04x \n", result, r1.mantissa, r1.exponent);
	printf("Expected (single precision computation): %.12f Mantissa: %06x Exponent: %04x \n", expected_sprec, r2.mantissa, r2.exponent);
	printf("Expected (double precision computation): %.12f Mantissa: %06x Exponent: %04x \n", expected_dprec, r3.mantissa, r3.exponent);
}
