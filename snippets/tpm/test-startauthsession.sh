#!/bin/sh

set -x

#
# Test that owner and index passwords work by
# 1. Setting up the owner password
# 2. Defining an nv index that can be satisfied by an:
#   a. Owner authorization
#   b. Index authorization
# 3. Using index and owner based auth during write/read operations
# 4. Testing that auth is needed or a failure occurs.
#

tpm2 clear

tpm2 changeauth -c o owner

# this satisfies both owner and index password, if we remove auth* then the index password won't work
# if we remove owner* then the password owner won't work
tpm2 nvdefine   0x1500015 -C o -s 32 \
  -a "policyread|policywrite|authread|authwrite|ownerread|ownerwrite" \
  -p "index" -P "owner"

# Use index password write/read, implicit -a
echo foo > nv.test_w
tpm2 nvwrite -Q 0x1500015 -P "index" -i nv.test_w
tpm2 nvread -Q 0x1500015 -P "index"

# Use index password write/read, explicit -a
tpm2 nvwrite -Q 0x1500015 -C 0x1500015 -P "index" -i nv.test_w
tpm2 nvread -Q 0x1500015 -C 0x1500015 -P "index"

# use owner password
tpm2 nvwrite -Q 0x1500015 -C o -P "owner" -i nv.test_w
tpm2 nvread -Q 0x1500015 -C o -P "owner"

# Check a bad password fails
tpm2 nvwrite -Q   0x1500015 -C 0x1500015 -P "wrong" -i nv.test_w 2>/dev/null
if [ $? -eq 0 ];then
	echo "nvwrite with bad password should fail!"
	exit 1
fi

nv_test_index=0x1500018

# Test nvextend and nvdefine with aux sessions
tpm2 clear

tpm2 createprimary -C o -c prim.ctx
tpm2 startauthsession -S enc_session.ctx --hmac-session -c prim.ctx

tpm2 changeauth -c o owner
tpm2 nvdefine -C o -P owner -a "nt=extend|ownerread|policywrite|ownerwrite" $nv_test_index -p nvindexauth -S enc_session.ctx

echo "foo" | tpm2 nvextend -C o -P owner -i- $nv_test_index -S enc_session.ctx

# show all the sessions made
tpm2_getcap handles-loaded-session
tpm2_getcap handles-saved-session
tpm2_getcap handles-persistent

# delete those sessions
tpm2 flushcontext enc_session.ctx
rm enc_session.ctx
rm prim.ctx

check=$(tpm2 nvread -C o -P owner $nv_test_index | xxd -p -c 64 | sed s/'^0*'//)
expected="1c8457de84bb43c18d5e1d75c43e393bdaa7bca8d25967eedd580c912db65e3e"
if [ "$check" != "$expected" ]; then
	echo "Expected setbits read value of \"$expected\", got \"$check\""
	exit 1
fi


