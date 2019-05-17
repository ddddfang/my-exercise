
function generator(){

    echo ".constants"

    i=0
    while [ $i -le 50 ]
    do
        t=$(printf "insert %d user%d mail_of_user%d" $i $i $i)
        echo "$t"
        i=$(( $i+1 ))
    done

    echo "select"
    echo ".btree"
    echo ".exit"
}


function collector(){
    echo "collector"    
}


generator | ./simpledb test.db


