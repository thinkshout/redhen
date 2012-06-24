<?php

/**
 * @file
 * Rules integration for RedHen Engagements.
 */

/**
 * Implements hook_rules_action_info().
 */
function redhen_engagement_rules_action_info() {
  $defaults = array(
		'parameter' => array(
			'score' => array(
				'type' => 'list<integer>',
				'label' => t('Engagement Score'),
				'options list' => 'redhen_engagement_score_options',
			),
		),
    'group' => t('Engagement'),
    'access callback' => 'redhen_engagement_access',
  );
  $actions['score_registration_engagement'] = $defaults + array(
    'label' => t('Score registration engagement'),
    'base' => 'redhen_engagement_rules_action_score_registration',
  );
	$actions['score_registration_engagement']['parameter']['registration'] = array(
		'type' => 'registration',
		'label' => t('Registration'),
	  'description' => t('The registration entity to be scored.'),
	);

  return $actions;
}

/**
 * Helper function for saving an engagement score when the Rule action is triggered.
 *
 * @TODO: This action depends on Registration, so it can't be located just in redhen_engagement.
 */
function redhen_engagement_rules_action_score_registration($score, $registration) {
	if (!empty($registration->anon_mail)) {
	  $query = new EntityFieldQuery;
  	$query
  	  ->entityCondition('entity_type', 'redhen_contact')
      ->propertyCondition('redhen_state', REDHEN_STATE_ACTIVE)
      ->fieldCondition('redhen_contact_email', 'value', $registration->anon_mail, '=');
  	$result = $query->execute();
  	if (!empty($result)) {
      $contact = redhen_contact_load(reset(array_keys($result['redhen_contact'])));
    }
	}
	elseif (!empty($registration->user_uid)) {
	  $user = user_load($registration->user_uid);
	  $contact = redhen_contact_load_by_user($user);
	}

	if (!empty($contact)) {
    // @todo: Should score be a single engagement score?
	  foreach($score as $engagement_score) {
      // @todo: Reconsider the engagement score name pattern.
  	  $engagement = redhen_engagement_create('engagement_score_' . $engagement_score, $contact->contact_id, 'Scored registration');
      redhen_engagement_save($engagement);
  	}
	}
}
