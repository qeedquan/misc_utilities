# Upon install Windows 11, to bypass network login, do:
# Shift-F10 on the first screen to open command line, then type in the command:
# OOBE\BYPASSNRO
# This will restart Windows 11 install and bypass the forced network login

# Windows 11 needs secure boot and TPM to install
# Use OVMF (UEFI) image for secure boot and TPM
# The OVMF_VARS image needs to be writable (it stores the BIOS settings), so copy the base image somewhere and mark it writable
# For the TPM, run swtpm to provide a software TPM emulator
OVMF_CODE="/usr/share/OVMF/OVMF_CODE_4M.secboot.fd"
OVMF_VARS="OVMF_VARS_4M.ms.fd"
SWTPM_SOCK="swtpm/swtpm-sock"

# Need smm for secure boot
ARCH="-M q35,smm=on"
CPU="-cpu host,hv_relaxed,hv_frequencies,hv_vpindex,hv_ipi,hv_tlbflush,hv_spinlocks=0x1fff,hv_synic,hv_runtime,hv_time,hv_stimer,hv_vapic -smp cores=8 -accel kvm"
MEM="-m 16G"
DRIVE="-drive file=win11.qcow2"
#NET="-nic user,model=e1000 -monitor stdio"
NET="-nic none"
CDROM="-cdrom Win11_25H2_English_x64_v2.iso"
TPM="-chardev socket,id=chrtpm,path=${SWTPM_SOCK} -tpmdev emulator,id=tpm0,chardev=chrtpm -device tpm-tis,tpmdev=tpm0"
SECURE_BOOT="-global driver=cfi.pflash01,property=secure,value=on"
BOOT="-drive if=pflash,format=raw,unit=0,file=${OVMF_CODE},readonly=on"
BOOT="$BOOT -drive if=pflash,format=raw,unit=1,file=${OVMF_VARS}"

OPTS="$ARCH $CPU $MEM $DRIVE $CDROM $TPM $SECURE_BOOT $BOOT $NET"

qemu-system-x86_64 $OPTS
