#include <stdio.h>
#include "rtl-sdr.h"

int main() {

	rtlsdr_dev_t* dev = NULL;
	unsigned char buf[500224];		// raw IQ
	float interleaved[500224];		// shifted IQ
	float rf[250112];			// RF power
	int n_read = 0;
	unsigned long i, j, k;
	FILE *fp;
	char buf2[20];

	rtlsdr_open( &dev, 0 );
	verbose_set_sample_rate( dev, 250000 );
	verbose_set_frequency( dev, 916500000 );   // center
//	verbose_auto_gain( dev );
	verbose_gain_set( dev, 600 );		// 60 dB
	verbose_ppm_set( dev, 54 );

	rtlsdr_reset_buffer( dev );

	while(1) {

		k = 0;
		rtlsdr_read_sync( dev, buf, 500224, &n_read );
		for (i=0; i<500224; i=i+2) {
			interleaved[i] = (float)buf[i] - 127.4;		// in-phase
			interleaved[i+1] = (float)buf[i+1] - 127.4;	// quadrature
			rf[k++] = interleaved[i]*interleaved[i] + interleaved[i+1]*interleaved[i+1];
		}

		/* detect presence of RF power */
		for (i=0;i<=k;i++) {
			if (rf[i] > 5000) {	// max RF power: 32768 

				fprintf(stderr, "%d: packet \n", time(0), i);

				/* Medtronic OOK packet = 130 characters of 6 bits each = 780 bits	*/
				/* - data rate of 16,000 bps ... thus record for 1/20th of second       */
				/* - RTLSDR sample rate of 250,000 ... thus save to disk 12,500 samples */
				
				sprintf(buf2, "packet_%d.txt", time(0));
				fp = fopen(buf2, "w");
				j = i - 100;
				while(j < i - 100 + 13000 && j < 250111) fprintf(fp, "%.0f ", rf[j++]);
				fclose(fp);
				if (j >= 250111) fprintf(stderr, "overflow detected\n");

				i = k + 1;
			} 
		}
		usleep(100);
		fprintf(stderr, ".");
	} 
}
