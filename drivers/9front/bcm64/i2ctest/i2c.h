typedef struct {
	int cfd, dfd;
} I2C;

int i2copen(I2C *, int);
int i2cclose(I2C *);

int i2csetslave(I2C *, int);

int i2csmbwd(I2C *, void *, int);
int i2csmbwb(I2C *, u8int);

int i2csmbrd(I2C *, u8int, void *, int);
int i2csmbrb(I2C *, u8int);
