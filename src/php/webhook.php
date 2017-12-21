<?php
/**
 * @file   webhook.php
 * @brief  execute webhook script
 * @author simpart
 */
namespace pkh;
require_once(__DIR__ . '/ttr/class.php');
require_once(__DIR__ . '/ttr/rest/require.php');
require_once(__DIR__ . '/conf.php');

$gpkh_msg = null;

try {
    if (2 > $argc) {
        throw new Exception('could not find post message');
    }
    $gpkh_msg = $argv[1];
    $cnf      = getConf();
    $rest = new \ttr\rest\RestSender($cnf['HOOK_URI']);
    $ret  = $rest->sendPost(
                $cnf['HOOK_PRM'],
                array(
                    CURLOPT_HTTPHEADER => array('Content-Type:application/json')
                )
            );
} catch (Exception $e) {
    echo $e->getMessage();
}

