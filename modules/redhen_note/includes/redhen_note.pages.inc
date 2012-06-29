<?php

/**
 * @file
 * Page callbacks for redhen ntoes.
 */

module_load_include('inc', 'redhen', 'includes/redhen.forms');

/*
 * Create new Redhen note object and call note form
 */
function redhen_note_add($entity, $entity_type) {
  $redhen_note = entity_get_controller('redhen_note')->create(array('type' => 'redhen_note'));
  $redhen_note->entity_type = $entity_type;
  $redhen_note->entity = $entity;
  module_load_include('inc', 'redhen_note', 'includes/redhen_note.forms');
  return drupal_get_form('redhen_note_form', $redhen_note);
}

/*
 * Show notes and actions
 */
function redhen_note_notes_page($entity, $entity_type) {

  list($entity_id, ,) = entity_extract_ids($entity_type, $entity);

  switch ($entity_type) {
    case 'redhen_contact':
      $path = 'contact';
      break;
    case 'redhen_org':
      $path = 'org';
      break;
  }

  $header = array(
    'Body',
    'Note Type',
    'Author',
    'updated' => array(
      'field' => 'updated',
      'type' => 'property',
      'data' => 'Updated',
      'sort' => 'asc',
      'specifier' => 'updated'
    ),
    'created' => array(
      'field' => 'created',
      'type' => 'property',
      'data' => 'Created',
      'sort' => 'asc',
      'specifier' => 'created'
    ),
    'Actions',
  );

  // need to ensure the query doesn't execute when posing the form
  $result = FALSE;
  if (!isset($_POST['form_id'])) {
    $bundle = '';
    $properties = isset($_GET['properties']) ? $_GET['properties'] : array();
    $properties['entity_id'] = $entity_id;
    $properties['entity_type'] = $entity_type;
    $fields = isset($_GET['fields']) ? $_GET['fields'] : array();
    $result = redhen_filter_query('redhen_note', $header, $bundle, $properties, $fields);
  }


  $rows = array();
  if ($result) {
    $notes = redhen_note_load_multiple(array_keys($result['redhen_note']));
    foreach ($notes as $note) {
      $actions = array(
        l('view', 'redhen/' . $path . '/' . $entity_id . '/note/' . $note->note_id . '/view'),
        l('edit', 'redhen/' . $path . '/' . $entity_id . '/note/' . $note->note_id . '/view/edit', array('query' => drupal_get_destination())),
        l('delete', 'redhen/' . $path . '/' . $entity_id . '/note/' . $note->note_id . '/view/delete', array('query' => drupal_get_destination())),
      );

      $body = $note->redhen_note_body[LANGUAGE_NONE][0]['value'];
      $type_tid = (empty($note->redhen_note_type) == 1) ? '' : $note->redhen_note_type[LANGUAGE_NONE][0]['tid'];
      $type = taxonomy_term_load($type_tid);

      $rows[] = array(
        'data' => array(
          substr($body, 0, 5) . ' ...',
          (isset($type->name)) ? $type->name : '',
          $note->author->name,
          format_date($note->updated, 'short'),
          format_date($note->created, 'short'),
          implode(' | ', $actions)
        )
      );
    }
  }
  else {
    // no results, set a message
    $rows[] = array(
      'data' => array(
        array(
          'data' => t('Sorry, there are no note that match your criteria.'),
          'colspan' => 7
        )
      )
    );
  }

  $render = array(
    'form' => drupal_get_form('redhen_filter_form', 'redhen_note'),
    'table' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows
    ),
    'pager' => array(
      '#theme' => 'pager'
    )
  );

  return $render;
}
