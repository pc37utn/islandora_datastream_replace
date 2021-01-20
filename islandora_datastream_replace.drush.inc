<?php
/**
 * @file
 * Drush command/hook implementation for replacing datastreams.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_datastream_replace_drush_command() {
  $commands = array();

  $commands['islandora_datastream_replace'] = array(
    'description' => dt('Replaces a datastream in all objects given a file list in a directory.'),
    'options' => array(
      'namespace' => array(
        'description' => dt('The namespace of the pids.'),
        'required' => TRUE,
      ),
      'source' => array(
        'description' => dt('The directory to get the datastreams and pid# from.'),
        'required' => TRUE,
      ),
     'dsid' => array(
        'description' => dt('The datastream id of the datastream.'),
        'required' => TRUE,
      ),
     ),
    'aliases' => array('idre'),
    'drupal dependencies' => array(
      'islandora',
    ),
    'examples' => array(
      'drush -u 1 islandora_datastream_replace --source=/tmp --dsid=MODS --namespace=test2 ' => dt('Replacing MODS datastream in object.'),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $commands;
}

/**
 * Batch process validation handler.
 */
function drush_islandora_datastream_replace_islandora_datastream_replace_validate() {
  $directory = drush_get_option('source');
  if (!is_writable($directory)) {
    return drush_set_error('This is not is a directory', dt('The specified source directory, !dir, is not valid.', array('!dir' => $directory)));
  }
}

/**
 * Command callback to replace datastreams.
 */
function drush_islandora_datastream_replace() {
  batch_set(islandora_datastream_replace_create_batch());
  drush_backend_batch_process();
}

/**
 * Constructs a batch used to update things via Drush.
 */
function islandora_datastream_replace_create_batch() {
  return array(
    'operations' => array(
      array('islandora_datastream_replace_batch_operation',
        array(
          drush_get_option('namespace'),
          drush_get_option('source'),
          drush_get_option('dsid'),
        ),
      ),
    ),
    'title' => t('Importing datastreams...'),
    'init_message' => t('Preparing to import datastream.'),
    'progress_message' => t('Time elapsed: @elapsed <br/>Estimated time remaining @estimate.'),
    'error_message' => t('An error has occurred.'),
    'file' => drupal_get_path('module', 'islandora_datastream_replace') . '/islandora_datastream_replace.drush.inc',
  );
}


/**
 * Constructs and performs the datastream replace operation.
 *
 * @param string $namespace
 *   The namespace to work in.
 * @param string $source
 *   The source directory to read the files from.
 * @param string $dsid
 *   The datastream id to be replaced.
 * @param array $context
 *   The context of the Drupal batch.
 */
function islandora_datastream_replace_batch_operation($namespace, $source, $dsid, &$context) {
  $sandbox = &$context['sandbox'];
  // limit type of DSID
  if (($dsid!='MODS')&&($dsid!='HOCR')&&($dsid!='OCR')&&($dsid!='TN')) {
        print " DSID is not enabled \n";
        exit();
      }
  //  query of source directory
  //  files should be in the same form as the output from the
  // islandora_datastream_export module
  //  namespace_pidnumber_dsid.ext
  $files = scandir($source);
  $nspart=$namespace.'_';
  //count expected files in this directory for sandbox
  $lscommand="ls -1 $source/*$dsid* | wc -l";
  $numfiles=exec($lscommand);
  // loop through files
  foreach ($files as $fil) {
    // eliminate the dot directories
    if ($fil === '.' || $fil === '..') {continue};
    // work with files that are not the '.used' only
    if (substr($fil,-5,5)!='.used') {
      // get name without extension
      $xbase=pathinfo($fil, PATHINFO_FILENAME);
      // save the extension
      $exten=pathinfo($fil, PATHINFO_EXTENSION);
      // get pieces of filename
      $xparts=explode('_',$xbase); 
      //find namespace equivalent
      $nstest=$xparts[0];
      $dsidtest=$xparts[2];
      if ($dsidtest!=$dsid) {
        print "dsid=$dsid  filename=$xbase \n";
        print " dsid is not in filename! \n";
        continue;
      }
      print "\n file = $xbase \n";
      //get number part of pid 
      $pidnum=$xparts[1];
      print "pidnum=$pidnum \n";
      if ($nstest!=$namespace) {
        print "\n extra namespace: $nstest \n";
        //continue;
      }
      // assemble filename that is like pid
      $pidname=$nstest.'_'.$pidnum.'_'.$dsid;
      //  assemble pid from filename parts
      $pid=$nstest.':'.$pidnum;      
      // prepare tuque and islandora includes
      module_load_include('inc', 'islandora', 'includes/mimetype.utils');
      module_load_include('inc', 'islandora', 'utilities');
      $object = islandora_object_load($pid);
      if (isset($object[$dsid])) {
        // assemble current source filename
        $source_directory = $source.'/'.$pidname.'.'.$exten;
        $mimetype = file_get_mimetype($source_directory);
        $filelabel = $pidname.'.'.$exten;
        $object[$dsid]->label = "$filelabel";
        $object[$dsid]->mimetype = $mimetype;
        $object[$dsid]->setContentFromFile($source_directory);
        $success = TRUE;
        rename($source_directory,"$source_directory".".used");
      }
      else {
        $success = FALSE;
      }
      if ($success) {
        drush_log(dt("Datastream replacement succeeded for @pid. ", array('@pid' => $object->id, )), 'success');
      }
      else {
        drush_log(dt("Datastream replacement failed for @pid. ", array('@pid' => $object->id, )), 'error');
      }
    }// end if right file
  }// end foreach
}// end function
