#!/bin/sh

set -x

# create an index with some size and write data to it
# verify the data we read back is the same
test_index() {
	CREATE=$1
	INDEX=$2
	SIZE=$3
	in=nv_$INDEX.bin
	out=nv_$INDEX.out
	if [ "$CREATE" -eq "1" ]; then
		tpm2_nvdefine $INDEX -C o -s $SIZE -a "ownerread|ownerwrite|policywrite"
	fi
	dd if=/dev/urandom of=$in bs=$SIZE count=1
	tpm2_nvwrite -Q $INDEX -C o -i $in
	tpm2_nvread -C o -s $SIZE $INDEX > $out
	sha256sum $in $out
}

# seal a index with pcr values
test_pcr() {
	INDEX=0x1500019
	in=pcr.bin
	
	tpm2_flushcontext -t
	tpm2_flushcontext -l

	echo foo > $in
	tpm2_pcrread -o srtm.pcrvalues sha256:0,1,2,3
	tpm2_createpolicy --policy-pcr -l sha256:0,1,2,3 -f srtm.pcrvalues -L srtm.policy
	
	tpm2_nvdefine $INDEX -C o -s 32 -L srtm.policy -a "policyread|policywrite"
	# writing it raw will fail
	#tpm2_nvwrite $INDEX -C o -i $in
	tpm2_nvwrite $INDEX -C $INDEX -P pcr:sha256:0,1,2,3=srtm.pcrvalues -i $in
	tpm2_nvread $INDEX -C $INDEX -P pcr:sha256:0,1,2,3=srtm.pcrvalues

	tpm2_nvundefine $INDEX
	
	rm -f *.bin *.policy *.pcrvalues
}

delete_index() {
	tpm2_nvundefine $1
}

stress_test_index() {
	for i in $(seq 1 4); do
		test_index 1 $i 2048
	done

	for i in $(seq 1 4); do
		for n in $(seq 5); do
			test_index 0 $i 2048
		done
	done

	for i in $(seq 1 4); do
		delete_index $i
	done

	rm -f *.bin *.out
}

stress_test_index
test_pcr
