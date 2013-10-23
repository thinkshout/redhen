<?php

/**
 * Page callback: Add a new donation to a host entity.
 *
 * @param string $entity_type
 *   The host entity type.
 * @param object $entity
 *   The host entity.
 *
 * @return array
 *   A render array
 *
 * @see redhen_donation_donate_page_access()
 * @see redhen_donation_menu()
 */
function redhen_donation_donate_page($entity_type, $entity) {
  list($entity_id) = entity_extract_ids($entity_type, $entity);
  if (redhen_donation_status($entity_type, $entity_id)) {
    $donation_type = redhen_donation_get_entity_donation_type($entity_type, $entity);
    $donation = entity_get_controller('redhen_donation')->create(array(
      'entity_type' => $entity_type,
      'entity_id' => $entity_id,
      'type' => $donation_type,
    ));
    return drupal_get_form('redhen_donation_form', $donation);
  }
  else {
    return t('Sorry, donations are no longer available for %name',
      array('%name' => entity_label($entity_type, $entity)));
  }
}

/**
 * Page callback for entity donation settings.
 *
 * @param $entity_type
 * @param $entity
 *
 * @return array
 *   Donation entity settings form.
 */
function redhen_donation_entity_settings_page($entity_type, $entity) {
  list($entity_id) = entity_extract_ids($entity_type, $entity);
  $settings = redhen_donation_entity_settings($entity_type, $entity_id);
  return drupal_get_form('redhen_donation_entity_settings_form', $settings, $entity_type, $entity_id);
}

/**
 * Page callback: Show a list of donations for a host entity.
 *
 * @param string $entity_type
 *   The host entity type.
 * @param object $entity
 *   The host entity.
 *
 * @return array
 *   A render array
 *
 * @see redhen_donation_administer_donations_access()
 * @see redhen_donation_menu()
 */
function redhen_donation_donations_page($entity_type, $entity) {
  //@TODO Remove stub
  return 'Donations page';
  $header = array(
    array(
      'data' => t('id'),
      'field' => 'donation_id',
      'type' => 'property',
      'specifier' => 'donation_id'
    ),
    array(
      'data' => t('User'),
      'field' => 'user_uid',
      'type' => 'property',
      'specifier' => 'user_uid'
    ),
    array(
      'data' => t('Created By'),
      'field' => 'author_uid',
      'type' => 'property',
      'specifier' => 'author_uid'
    ),
    array(
      'data' => t('Created'),
      'field' => 'created',
      'sort' => 'desc',
      'type' => 'property',
      'specifier' => 'created'
    ),
    array('data' => t('Actions')),
  );

  list($entity_id) = entity_extract_ids($entity_type, $entity);
  $label = entity_label($entity_type, $entity);

  $query = new EntityFieldQuery;
  $result = $query
    ->entityCondition('entity_type', 'registration')
    ->propertyCondition('entity_id', $entity_id)
    ->propertyCondition('entity_type', $entity_type)
    ->pager(20)
    ->tableSort($header)
    ->execute();

  if (!empty($result['registration'])) {
    $registrations = registration_load_multiple(array_keys($result['registration']));
    $rows = array();

    foreach ($registrations as $registration) {
      $wrapper = entity_metadata_wrapper('registration', $registration);
      $author = $wrapper->author->value();
      $state = $wrapper->state->value();

      $author_col = '';
      if ($registration->author_uid) {
        $uri = entity_uri('user', $author);
        $author_col = l($author->name, $uri['path']);
      }

      $user_col = '';
      if ($registration->user_uid) {
        $user = user_load($registration->user_uid);
        $uri = entity_uri('user', $user);
        $user_col = l($user->name, $uri['path']);
      }

      $actions = array();
      if (entity_access('view', 'registration', $registration)) {
        $actions[] = l(t('View'), 'registration/' . $registration->registration_id);
      }
      if (entity_access('update', 'registration', $registration)) {
        $actions[] = l(t('Edit'), 'registration/' . $registration->registration_id . '/edit', array('query' => drupal_get_destination()));
      }
      if (entity_access('delete', 'registration', $registration)) {
        $actions[] = l(t('Delete'), 'registration/' . $registration->registration_id . '/delete', array('query' => drupal_get_destination()));
      }

      $rows[] = array(
        l($registration->registration_id, 'registration/' . $registration->registration_id),
        l($wrapper->mail->value(), 'mailto:' . $wrapper->mail->value()),
        $user_col,
        $author_col,
        $registration->count,
        format_date($registration->created),
        ($state ? filter_xss_admin(entity_label('registration_state', $state)) : ''),
        implode(' | ', $actions)
      );
    }

    $settings = registration_entity_settings($entity_type, $entity_id);

    $table = array(
      'header' => $header,
      'rows' => $rows
    );
    if ($settings['capacity'] != 0) {
      $table['caption'] = t('List of registrations for %title. !count of !capacity spaces are filled.', array(
        '%title' => $label,
        '!count' => '<strong>' . registration_event_count($entity_type, $entity_id) . '</strong>',
        '!capacity' => '<strong>' . $settings['capacity'] . '</strong>'
      ));
    }
    else {
      $table['caption'] = t('List of registrations for %title. !count spaces are filled.', array(
        '%title' => $label,
        '!count' => '<strong>' . registration_event_count($entity_type, $entity_id) . '</strong>',
      ));
    }

    $out = theme('table', $table) . theme('pager');
  }
  else {
    $out = t('There are no registrants for %name',
      array('%name' => $label));
  }

  return $out;
}
