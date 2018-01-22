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
    $prm_jsn  = json_decode($argv[1]);
    $gpkh_msg = $prm_jsn->message;;
    $cnf      = getConf();
    
    $rst_prm  = array();
    foreach ($cnf['HOOK_PRM'] as $cnf_key => $cnf_val) {
        $rst_prm[$cnf_key] = $cnf_val;
    }
    foreach ($prm_jsn as $jsn_key => $jsn_val) {
        $rst_prm[$jsn_key] = $jsn_val;
    }
    
    $rest = new \ttr\rest\RestSender($cnf['HOOK_URI']);
    $ret  = $rest->sendPost(
                $rst_prm,
                array(
                    CURLOPT_HTTPHEADER => array('Content-Type:application/json')
                )
            );
} catch (Exception $e) {
    echo $e->getMessage();
}

