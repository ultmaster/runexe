../cmake-build-debug/runexe -xml -o output.tmp ./args.o 1 2 3
if diff output.tmp args.expected; then
    echo "args OK"
fi

../cmake-build-debug/runexe -xml ./memory1.o
if [[ $? -eq 0 ]]; then
    echo "memory1 OK"
fi

../cmake-build-debug/runexe -xml -m 128M ./memory2.o
if [[ $? -eq 0 ]]; then
    echo "memory2 OK"
fi

../cmake-build-debug/runexe -xml -m 128M ./memory3.o
if [[ $? -eq 0 ]]; then
    echo "memory3 OK"
fi

../cmake-build-debug/runexe -m 512M ./memory3.o
if [[ $? -eq 0 ]]; then
    echo "memory3 extended OK"
fi

../cmake-build-debug/runexe -xml -t 1s ./while1.o
if [[ $? -eq 0 ]]; then
    echo "while1 OK"
fi

../cmake-build-debug/runexe -xml --interactor="-t 2s ./canteenInter.o ./canteen.in ./canteen.tmp" ./canteenSol.o
if [[ $? -eq 0 ]]; then
    echo "interactor OK"
fi
