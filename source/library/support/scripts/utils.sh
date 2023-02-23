# Author: Dominic Clifton

error() {
    MESSAGE=$1
    echo ERROR $MESSAGE
    exit -1
}

wait_for_external_flash() {
    echo -n "Looking for device with external flash."
    
    ATTEMPTS_REMAINING=$[MAXIMUM_ATTEMPTS]
    
    while true; do
        
        DEVICE=`$DFU_UTIL -l 2> /dev/null | grep "0483:df11" | grep "@External Flash" | cut -c24-`
        echo -n "."
        if [ "$DEVICE" == "" ]; then
            ATTEMPTS_REMAINING=$[ATTEMPTS_REMAINING - 1]
            
            if [ $ATTEMPTS_REMAINING -eq 0 ]; then
                echo -n "Device not found, retry? (y/n)"
                read RESPONSE
                shopt -s nocasematch
                if [ "$RESPONSE" != "y" ]; then
                    error "Aborted"
                else
                    ATTEMPTS_REMAINING=$[MAXIMUM_ATTEMPTS]
                fi
            else
                sleep 1
            fi
        else
            echo "Found"
            break
        fi
    done
    
    echo $DEVICE
}

wait_for_internal_flash() {
    echo -n "Looking for device with internal flash."
    
    ATTEMPTS_REMAINING=$[MAXIMUM_ATTEMTPTS]
    
    while true; do
        
        DEVICE=`$DFU_UTIL -l 2> /dev/null | grep "0483:df11" | grep "@Internal Flash" | cut -c24-`
        echo -n "."
        if [ "$DEVICE" == "" ]; then
            ATTEMPTS_REMAINING=$[ATTEMPTS_REMAINING - 1]
            
            if [ $ATTEMPTS_REMAINING -eq 0 ]; then
                echo -n "Device not found, retry? (y/n)"
                read RESPONSE
                if [ "$RESPONSE" != "y" ]; then
                    error "Aborted"
                else
                    ATTEMPTS_REMAINING=$[MAXIMUM_ATTEMTPTS]
                fi
            else
                sleep 1
            fi
        else
            echo "Found"
            break
        fi
    done
    
    echo $DEVICE
}
