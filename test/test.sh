#!/bin/bash


function getInfos {
    file=$1
    field=$2
    IFS=$'\n'
    lines=($line)
    
    while IFS= read -r line
    do
         if [[ $line == '#'* ]];
               then
               IFS=" "
               words=($line)
                    if [[ ${words[0]} == "#"$field ]];
                        then
                        value=""
                        isMoreThanOneWord=false
                           for word in ${words[@]}; do
                            if [[ $word != ${words[0]} ]];
                            then
                                if [[ $word == ${words[2]} ]];
                                then
                                    isMoreThanOneWord=true
                                fi
                                
                                if [[ $isMoreThanOneWord == true ]];
                                then
                                    value+=" "
                                fi
                                
                                value+=$word
                                #echo $value
                            fi
                            done
                        fi
                        IFS=$'\n'
               fi
       
               
    done < "$file"
}





simulator=$1

for binary in testcases/Bin/*; do

    testName=$(basename -- "${binary%.*}")
    echo $testName

    cat "Bonjour" | ./$1 $binary
    RETCODE=$?
    #echo $RETCODE
    ############################################## change this to proper assembly file
    AssemblyFile=testcases/Assembly/"$testName".s

    #ExpectedOutput
    getInfos $AssemblyFile ExpectedOutput
    ExpectedOutput=$value
    echo $ExpectedOutput

    #TestID
    getInfos $AssemblyFile TestID
    TestId=$value

    #Instruction
    getInfos $AssemblyFile Instruction
    Instruction=$value

    #Author
    getInfos $AssemblyFile Author
    Author=$value
    
    getInfos $AssemblyFile Description
    Description=$value
    
    Status=Fail
    
    A=$ExpectedOutput
    if [[ $A =~ ^[+-]?[0-9]+$ ]]; then
    while  [[ $A < 0 ]]
    do
            A=$(($A+256))
    done
    fi
    
    ExpectedOutput=$A
    
    if [[ "$(($RETCODE%256))" == $ExpectedOutput ]]; then
        Status=Success
    fi

    outputline="$testName, $Instruction, $Status, $Author, [Expected: $ExpectedOutput - Actual: $RETCODE - $Description]"
    
    
    printf outputline >> output
    
    echo $outputline
    
    echo $outputline >> text.csv
    
done


