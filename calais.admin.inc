<?php
/*
  Copyright (C) 2008 by Phase2 Technology.
  Author(s): Frank Febbraro, Irakli Nadareishvili

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY. See the LICENSE.txt file for more details.

  $Id$
 */
/**
 * @file
 */

/**
 * Build the administration page for Calais and Content Types
 */
function calais_admin_settings() {

  $form = array();

  calais_build_entity_settings($form);
  calais_build_nodetype_settings($form);

  $form['#submit'][] = 'calais_admin_settings_submit';

  $form = system_settings_form($form);
  return $form;
}

/**
 * Build the Entity selector. Used for Entity suppression.
 */
function calais_build_entity_settings(&$form, $type = 'global', $name = 'Global', $allow_disable = FALSE) {
  $entity_attributes = array();
  $entities = array_keys(calais_get_entity_vocabularies());
  sort($entities);

  $disabled = FALSE;
  if ($allow_disable) {
    $var_name = "calais_use_global_{$type}";
    $disabled = variable_get($var_name, TRUE);

    $form[$var_name] = array(
      '#type' => 'checkbox',
      '#title' => t('Use Calais Global Entity defaults'),
      '#default_value' => $disabled,
      '#description' => t('If checked, this content type will use the Global Calais Entities.'),
    );

    if ($disabled) {
      $entity_attributes['style'] = 'display:none';
    }

    $field_id = 'edit-'. str_replace(array('][', '_', ' '), '-', $var_name);
    drupal_add_js("$(document).ready(function() {
      $('#$field_id').click(function() {
        $('.{$type}_entities').toggle();
      });
    });", 'inline');
  }

  $params = array('@name' => $name);
  $form["calais_entity_settings_{$type}"] = array(
    '#type' => 'fieldset',
    '#title' => t('@name Calais Entities', $params),
    '#attributes' => array_merge(array('class' => "{$type}_entities"), $entity_attributes),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form["calais_entity_settings_{$type}"]["calais_applied_entities_{$type}"] = array(
    '#type' => 'checkboxes',
    '#title' => t('Which Calais Entities do you wish to use?'),
    '#default_value' => variable_get("calais_applied_entities_{$type}", $entities),
    '#options' => drupal_map_assoc($entities),
    '#description' => t('These are the entities that will get applied to content. If an Entity is not selected then the Calais suggestions will be ignored'),
  );
}

/**
 * Build the node type settings form for Calais integration.
 */
function calais_build_nodetype_settings(&$form) {
  node_types_rebuild();

  $node_types = node_get_types('types', NULL, TRUE);
  $options = array(
    CALAIS_PROCESS_NO => t('Not processed by Calais (default)'),
    CALAIS_PROCESS_MANUAL => t('Have keywords be suggested (manual term association)'),
    CALAIS_PROCESS_AUTO => t('Have keywords automatically applied on every update'),
    CALAIS_PROCESS_AUTO_ONCE => t('Have keywords automatically applied on creation only, and suggested on updates'),
  );

  foreach ($node_types as $nt) {
    $key = drupal_strtolower($nt->type);
    $name = $nt->name;
    $param_name = array('@node' => $name);
    
    $form["calais_{$key}_settings"] = array(
      '#type' => 'fieldset',
      '#title' => t('Processing Settings for @node', $param_name),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form["calais_{$key}_settings"]["calais_node_{$key}_process"] = array(
      '#type' => 'radios',
      '#parents' => array('calais_node_'. $key .'_process'),
      '#title' => t('Calais Processing'),
      '#default_value' => variable_get("calais_node_{$key}_process", CALAIS_PROCESS_NO),
      '#options' => $options,
    );
    $form["calais_{$key}_settings"]["calais_api_allow_searching_{$key}"] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow Calais Searching'),
      '#default_value' => variable_get("calais_api_allow_searching_{$key}", NULL),
      '#description' => t('Indicates whether future searches can be performed on the extracted metadata by Calais'),
    );
    $form["calais_{$key}_settings"]["calais_api_allow_distribution_{$key}"] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow Calais Distribution'),
      '#default_value' => variable_get("calais_api_allow_distribution_{$key}", NULL),
      '#description' => t('Indicates whether the extracted metadata can be distributed by Calais'),
    );
    $form["calais_{$key}_settings"]["calais_threshold_{$key}"] = array(
      '#type' => 'textfield',
      '#size' => 5,
      '#title' => t('Relevancy Threshold'),
      '#default_value' => variable_get("calais_threshold_{$key}", 0.0),
      '#description' => t('The relevancy threshold for Calais term association. The relevancy score is based on an importance detection for each unique entity and assigns a relevance score in the range 0 - 1 (0 being the least relevant; 1 being the most relevant; The lower the threshold the more terms will appear).'),
    );

    calais_build_entity_settings($form["calais_{$key}_settings"], $key, $name, TRUE);
  }
}

/**
 * Have to override so that we can manage vocabulary - node_type relationships.
 *
 * @param unknown_type $form_id
 * @param unknown_type $form_state
 */
function calais_admin_settings_submit($form, &$form_state) {

  system_settings_form_submit($form, $form_state);

  // Also, set vocabulary-node relationships
  $node_types = node_get_types('types', NULL, TRUE);
  $all_vocabularies = calais_get_entity_vocabularies();
  foreach ($node_types as $nt) {
    $key = drupal_strtolower($nt->type);
    $state = variable_get('calais_node_'. $key .'_process', CALAIS_PROCESS_NO);
    foreach ($all_vocabularies as $entity => $vid) {
      // Clean-up
      db_query("DELETE FROM {vocabulary_node_types} WHERE vid='%d' and type='%s'", $vid, $key);
    }
    if ($state !== CALAIS_PROCESS_NO) {
      // assign all configured calais vocabs to this node type
      $node_vocabularies = calais_get_entity_vocabularies($key);
      foreach ($node_vocabularies as $entity => $vid) {
        db_query("INSERT INTO {vocabulary_node_types} (vid, type) values('%d','%s') ", $vid, $key);
      }
    }
  }
}


/**
 * Build the administration page for submitting all nodes of a specific Content Types to Calais.
 */
function calais_bulk_process() {

  $form = array();

  $node_types = node_get_types('types', NULL, TRUE);
  $node_types = array();
  foreach (node_get_types('types', NULL, TRUE) as $type) {
    $node_types[$type->type] = $type->name;
  }

  $form["calais_bulk_type"] = array(
    '#type' => 'select',
    '#title' => t('Content Type'),
    '#options' => $node_types,
    '#description' => t('This Content Type will have all system Nodes bulk submitted to Calais for processing.'),
  );
  
  $form["calais_bulk_limit"] = array(
    '#type' => 'select',
    '#title' => t('Nodes per batch'),
    '#options' => drupal_map_assoc(array('1', '5', '10', '25', '50', '100')),
    '#description' => t('The number of Nodes to process per batch.'),
  );

  $form["calais_bulk_throttle"] = array(
    '#type' => 'checkbox',
    '#title' => t('Throttle processing'),
    '#description' => t('Calais implements a throttle on the number of requests per second. If you see @error messages, you should throttle the requests to Calais so that all nodes will get processed.', array('@error' => "Developer Over Qps")),
  );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Bulk Process'));

  $form['#submit'][] = 'calais_bulk_process_submit';

  return $form;
}

/**
 * Submit handler for the Bulk Processing Form.
 */
function calais_bulk_process_submit($form, &$form_state) {

  $values = $form_state['values'];
  $type = $values['calais_bulk_type'];
  $limit = $values['calais_bulk_limit'];
  $throttle = $values['calais_bulk_throttle'];
 
  $batch = array(
    'operations' => array(
      array('calais_batch_process', array($type, $limit, $throttle)),
    ),
    'finished' => 'calais_batch_finished',
    'title' => t('Calais Bulk Processing'),
    'init_message' => t('Bulk processing of nodes is starting.'),
    'error_message' => t('Calais Bulk Processing has encountered an error.'),
  );
  batch_set($batch);
}

/**
 * Process nodes in bulk by submitting blocks to Calais for metadata association.
 *  
 * @param type
 *   The node type that we want to process
 *
 * @param limit
 *   The number of nodes to process in each pass.
 *
 * @param throttle
 *   Flag indicating if Calais requests should be throttled (to stay below quota limits).
 *
 * @param context
 *   $context is an array that will contain information about the
 *   status of the batch. The values in $context will retain their
 *   values as the batch progresses.
 *
 * @param $context['sandbox']
 *   Use the $context['sandbox'] rather than $_SESSION to store the
 *   information needed to track information between successive calls.
 *   The values in the sandbox will be stored and updated in the database
 *   between http requests until the batch finishes processing. This will
 *   avoid problems if the user navigates away from the page before the
 *   batch finishes.
 *
 * @param $context['results']
 *   The array of results gathered so far by the batch processing.
 *   The current operation can append its own.
 *
 * @param $context['message']
 *   A text message displayed in the progress page.
 *
 * @param $context['finished']
 *   A float number between 0 and 1 informing the processing engine
 *   of the completion level for the operation.
 *   1 (or no value explicitly set) means the operation is finished
 *   and the batch processing can continue to the next operation.
 */
function calais_batch_process($type, $limit, $throttle, &$context) {
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['current_node'] = 0;
    $context['sandbox']['max'] = db_result(db_query("SELECT COUNT(DISTINCT nid) FROM {node} WHERE type = '%s'", $type));
  }
  
  // With each pass through the callback, retrieve the next group of nids.
  $current = $context['sandbox']['current_node'];
  $result = db_query_range("SELECT nid FROM {node} WHERE type = '%s' AND nid > %d ORDER BY nid ASC", $type, $current, 0, $limit);
  while ($row = db_fetch_array($result)) {

    $node = node_load($row['nid'], NULL, TRUE);
    calais_process_node($node, CALAIS_PROCESS_AUTO);

    if($throttle) {
      usleep(10000);
    }

    $context['results'][] = $node->title;
    $context['sandbox']['progress']++;
    $context['sandbox']['current_node'] = $node->nid;
    $context['message'] = t('Now processing %node', array('%node' => $node->title));
  }

  // Inform the batch engine that we are not finished,
  // and provide an estimation of the completion level we reached.
  if ($context['sandbox']['progress'] != $context['sandbox']['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }  
}

/**
 * Batch 'finished' callback
 */
function calais_batch_finished($success, $results, $operations) {
  if ($success) {
    $message = count($results) .' nodes processed.';
    $message .= theme('item_list', $results);
  }
  else {
    $error_operation = reset($operations);
    $message = 'An error occurred while processing '. $error_operation[0] .' with arguments :'. print_r($error_operation[0], TRUE);
  }
  drupal_set_message($message);
}