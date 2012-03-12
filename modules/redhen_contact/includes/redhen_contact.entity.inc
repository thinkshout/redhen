<?php
/**
 * @file
 * Redhen Contact entity classses
 */

/**
 * The class used for contact entities.
 */
class RedhenContact extends Entity {

  public
    $first_name = '',
    $last_name = '',
    $name,
    $email,
    $contact_id = NULL,
    $user_uid = NULL,
    $user,
    $author_uid = NULL,
    $redhen_state = REDHEN_STATE_ACTIVE,
    $engagement_score = NULL,
    $type = '',
    $created = '',
    $changed = '';


  public function __construct(array $values = array()) {
    parent::__construct($values, 'redhen_contact');

    // ensure all field values are loaded
    // @todo: Figure out why we need this here.
    if ($this->contact_id) {
      field_attach_load('redhen_contact', array($this->contact_id => $this));
    }

    $this->name = $this->name();
    $this->email = $this->defaultEmail();
    $this->user = $this->user();
  }

  /**
   * Override buildContent() to add contact properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $content['name'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Name'),
      '#items' => array(
        array(
          '#markup' => $this->name
        ),
       ),
      '#classes' => 'field field-label-inline clearfix',
    );
    $content['redhen_state'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('State'),
      '#items' => array(
        array(
          '#markup' => ($this->redhen_state == REDHEN_STATE_ACTIVE) ? t('Active') : t('Inactive')
        ),
       ),
      '#classes' => 'field field-label-inline clearfix',
    );

    if($this->user) {
      $user_uri = entity_uri('user', $this->user);
      $content['user'] = array(
        '#theme' => 'redhen_property_field',
        '#label' => t('Drupal User'),
        '#items' => array(
          array(
            '#markup' => l($this->user->name, $user_uri['path'])
          ),
         ),
        '#classes' => 'field field-label-inline clearfix',
      );
    }

    if ($this->engagement_score) {
      $content['redhen_engagement'] = array(
        '#theme' => 'redhen_property_field',
        '#label' => t('Engagement Score'),
        '#items' => array(
          array(
            '#markup' => $this->engagement_score
          ),
         ),
        '#classes' => 'field field-label-inline clearfix',
      );
    }

    return entity_get_controller($this->entityType)
			->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Specifies the default label, which is picked up by label() by default.
   */
  protected function defaultLabel() {
    $type = redhen_contact_get_types($this->type);
    return $type->label . ': ' . $this->name;
  }

  /**
   * Specifify URI
   */
  protected function defaultUri() {
    return array('path' => 'redhen/contact/' . $this->identifier());
  }

  /**
   * Return the contact formatted name, by default a concatenation of first and
   * last names. Can be changed by overriding this class or implementing
   * hook_redhen_contact_name_alter().
   *
   * @return string
   */
  protected function name() {
    $name =  $this->first_name . ' ' . $this->last_name;
    $context = array(
      'contact' => clone $this,
    );
    drupal_alter('redhen_contact_name', $name, $context);
    return $name;
  }

  /**
   * Return default email from email field.
   *
   * @return string
   */
  protected function defaultEmail() {
    if (!empty($this->{REDHEN_CONTACT_EMAIL_FIELD})) {
      foreach($this->{REDHEN_CONTACT_EMAIL_FIELD}[LANGUAGE_NONE] as $email) {
        if ($email['default']) {
          return $email['value'];
        }
      }
    }
    return '';
  }

  /**
   * Determine if contact has a given email.
   *
   * @param $email
   * @return bool
   */
  public function hasEmail($email) {
    if (!empty($this->{REDHEN_CONTACT_EMAIL_FIELD})) {
      foreach($this->{REDHEN_CONTACT_EMAIL_FIELD}[LANGUAGE_NONE] as $contact_email) {
        if ($contact_email['value'] === $email) {
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  /**
   * Sets an email for a contact.
   *
   * @param $email
   * @param int $default
   * @param int $bulk
   * @param int $hold
   * @param int $label_id
   */
  public function setEmail($email, $default = 1, $bulk = 1, $hold = 0, $label_id = 0) {
    $this->{REDHEN_CONTACT_EMAIL_FIELD}[LANGUAGE_NONE][] = array(
      'value' => $email,
      'default' => $default,
      'bulk' => $bulk,
      'hold' => $hold,
      'label_id' => $label_id
    );
  }

  /**
   * Return the active user linked to this contact.
   *
   * @return mixed|null
   */
  public function user() {
    $user = NULL;
    if ($this->user_uid) {
      $user = user_load($this->user_uid);
    }
    return $user;
  }

  /**
   * Set the active user for a contact.
   *
   * @param $user
   * @return mixed
   */
  public function setUser($user) {
    $contact_user = $this->contactUser();

    // no user connection, create
    if (!$contact_user) {
      db_insert('redhen_contact_user')
        ->fields(array(
          'contact_id' => $this->contact_id,
          'user_uid' => $user->uid,
          'created' => REQUEST_TIME,
          'updated' => REQUEST_TIME
          ))
        ->execute();
    }
    // same user, bail. doesn't matter if connection is active or inactive
    else if ($contact_user->user_uid != $user->uid) {
      // update with new user
      db_update('redhen_contact_user')
        ->fields(array(
            'user_uid' => $user->uid,
            'redhen_state' => REDHEN_STATE_ACTIVE,
            'updated' => REQUEST_TIME
          ))
        ->condition('contact_id', $this->contact_id)
        ->execute();
    }

    $this->user = $user;
  }

  /**
   * Private helper to return relationship between a contact and user.
   *
   * @return array
   */
  private function contactUser() {
    $user_info = array();
    $result = db_select('redhen_contact_user', 'cu')
      ->fields('cu')
      ->condition('contact_id', $this->contact_id, '=')
      ->execute();
    if ($result) {
      $user_info = reset($result->fetchAllAssoc('user_uid'));
    }
    return $user_info;
  }

}


/**
 * Controls metadata for Redhen contacts
 *
 * Redhen utilizes custom property attributes to determine if a property
 * should be available as a filter on the listing page. These attributes are
 *   filter: TRUE to add as a filter.
 *   field_type: textfield, select, etc.
 *   options: array of options for seelct field types.
 */
class RedhenContactMetadataController extends EntityDefaultMetadataController {

  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['created'] = array(
      'label' => t("Created"),
      'description' => t("The date the contact was created."),
      'type' => 'date',
      'schema field' => 'created',
    );
    $properties['updated'] = array(
      'label' => t("Updated"),
      'description' => t("The date the contact was updated."),
      'type' => 'date',
      'schema field' => 'updated',
    );
    $properties['user'] = array(
      'label' => t("User"),
      'type' => 'user',
      'description' => t("The Drupal user assocaited with the contact."),
      'schema field' => 'user_uid'
    );
    $properties['author'] = array(
      'label' => t("Author"),
      'type' => 'user',
      'description' => t("The author of the contact record."),
      'schema field' => 'author_uid'
    );

    $properties['first_name'] = array_merge($properties['first_name'], array(
      'label' => t('First Name'),
      'filter' => TRUE,
      'field_type' => 'textfield'
    ));
    $properties['last_name'] += array(
      'label' => t('Last Name'),
      'filter' => TRUE,
      'field_type' => 'textfield'
    );

    $properties['redhen_state'] += array(
      'label' => t('State'),
      'filter' => TRUE,
      'field_type' => 'select',
      'options' => array(
        REDHEN_STATE_ACTIVE => t('Active'),
        REDHEN_STATE_ARCHIVED => t('Archived')
      )
    );

    return $info;
  }

}

