<?php

namespace Drupal\redhen_contact\Entity;

use Drupal\Core\Entity\EntityStorageInterface;
use Drupal\Core\Field\BaseFieldDefinition;
use Drupal\Core\Entity\ContentEntityBase;
use Drupal\Core\Entity\EntityChangedTrait;
use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\redhen_contact\ContactInterface;
use Drupal\user\UserInterface;

/**
 * Defines the Contact entity.
 *
 * @ingroup redhen_contact
 *
 * @ContentEntityType(
 *   id = "redhen_contact",
 *   label = @Translation("Contact"),
 *   label_singular = @Translation("contact"),
 *   label_plural = @Translation("contacts"),
 *   label_count = @PluralTranslation(
 *     singular = "@count contact",
 *     plural = "@count contact",
 *   ),
 *   bundle_label = @Translation("Contact type"),
 *   handlers = {
 *     "view_builder" = "Drupal\Core\Entity\EntityViewBuilder",
 *     "list_builder" = "Drupal\redhen_contact\ContactListBuilder",
 *     "views_data" = "Drupal\views\EntityViewsData",
 *
 *     "form" = {
 *       "default" = "Drupal\redhen_contact\Form\ContactForm",
 *       "add" = "Drupal\redhen_contact\Form\ContactForm",
 *       "edit" = "Drupal\redhen_contact\Form\ContactForm",
 *       "delete" = "Drupal\redhen_contact\Form\ContactDeleteForm",
 *     },
 *     "access" = "Drupal\redhen_contact\ContactAccessControlHandler",
 *     "route_provider" = {
 *       "html" = "Drupal\redhen_contact\ContactHtmlRouteProvider",
 *     },
 *   },
 *   base_table = "redhen_contact",
 *   revision_table = "redhen_contact_revision",
 *   admin_permission = "administer contact entities",
 *   entity_keys = {
 *     "id" = "id",
 *     "revision" = "revision_id",
 *     "bundle" = "type",
 *     "uuid" = "uuid",
 *     "langcode" = "langcode",
 *     "status" = "status",
 *   },
 *   links = {
 *     "canonical" = "/redhen/contact/{redhen_contact}",
 *     "add-form" = "/redhen/contact/add/{redhen_contact_type}",
 *     "edit-form" = "/redhen/contact/{redhen_contact}/edit",
 *     "delete-form" = "/redhen/contact/{redhen_contact}/delete",
 *     "collection" = "/redhen/contact",
 *   },
 *   bundle_entity_type = "redhen_contact_type",
 *   field_ui_base_route = "entity.redhen_contact_type.edit_form"
 * )
 */
class Contact extends ContentEntityBase implements ContactInterface {
  use EntityChangedTrait;

  /**
   * {@inheritdoc}
   */
  public function label() {
    return $this->getFullName();
  }

  /**
   * {@inheritdoc}
   */
  public function getType() {
    return $this->bundle();
  }

  /**
   * {@inheritdoc}
   */
  public function getFullName() {
    $first_name = $this->get('first_name')->value;
    $middle_name = $this->get('middle_name')->value;
    $last_name = $this->get('last_name')->value;
    $name = $first_name . (empty($middle_name) ? '' : ' ') . $middle_name . (empty($first_name) ? '' : ' ') . $last_name;
    // Allow other modules to alter the full name of the contact.
    \Drupal::moduleHandler()->alter('redhen_contact_name', $name, $this);
    return $name;
  }

  /**
   * {@inheritdoc}
   */
  public function setName($name) {
    $this->set('name', $name);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getEmail() {
    return $this->get('email')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setEmail($email) {
    $this->set('email', $email);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getCreatedTime() {
    return $this->get('created')->value;
  }

  /**
   * {@inheritdoc}
   */
  public function setCreatedTime($timestamp) {
    $this->set('created', $timestamp);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getUser() {
    return $this->get('uid')->entity;
  }

  /**
   * {@inheritdoc}
   */
  public function getUserId() {
    return $this->get('uid')->target_id;
  }

  /**
   * {@inheritdoc}
   */
  public function setUserId($uid) {
    $this->set('uid', $uid);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function setUser(UserInterface $account) {
    $this->set('uid', $account->id());
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function isActive() {
    return (bool) $this->getEntityKey('status');
  }

  /**
   * {@inheritdoc}
   */
  public function setActive($active) {
    $this->set('status', $active ? REDHEN_CONTACT_INACTIVE : REDHEN_CONTACT_ACTIVE);
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function preSave(EntityStorageInterface $storage) {
    parent::preSave($storage);
    $config = \Drupal::config('redhen_contact.settings');
    $user = $this->getUser();
    $email = $this->getEmail();
    // Ensure we want to connect Contact to a Drupal user, there is no user
    // connected currently, and we have an email value.
    if ($config->get('connect_users') && !$user && $email) {
      $user = user_load_by_mail($email);
      if ($user) {
        $this->setUser($user);
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function postSave(EntityStorageInterface $storage, $update = TRUE) {
    parent::postSave($storage, $update);

    // Get RedHen Contact settings.
    $config = \Drupal::config('redhen_contact.settings');
    $user = $this->getUser();
    $email = $this->getEmail();
    // If we're mirroring the Contact's email address and we have a user and
    // email - set user's email address to that of the Contact.
    if ($config->get('connect_users') && $user && $email) {
      $user->setEmail($email);
      $user->save();
    }
  }

  /**
   * Load a contact record from a user account.
   *
   * @param object $account
   *   User object.
   * @param bool $status
   *   Redhen status. Defaults to active.
   *
   * @return mixed
   *   Contact or FALSE if not found.
   */
  public static function loadByUser($account, $status = TRUE) {
    $contact = &drupal_static(__FUNCTION__ . $account->id(), FALSE);

    // If we don't have a cached Contact and we have a uid to load the Contact
    // by, proceed.
    if (!$contact && !empty($account->id())) {

      // Find Contacts linked to the current Drupal User.
      $query = \Drupal::entityQuery('redhen_contact');
      $query->condition('uid', $account->id(), '=');
      $query->condition('status', $status);
      $results = $query->execute();

      // If we find a Contact, load and return it.
      if (!empty($results)) {
        // There should always be only a single active user linked to an account.
        $contact = Contact::load(reset($results));
      }
    }

    return $contact;
  }

  /**
   * Load all Contact entities for a given email address.
   *
   * @param string $email
   *   Required: an email address.
   * @param bool $status
   *   RedHen status. Defaults to active.
   *
   * @return array|bool
   *   An array of RedHen Contact entities or FALSE if no match found.
   */
  public static function loadByMail($email, $status = TRUE) {
    $contacts = &drupal_static(__FUNCTION__ . $email, FALSE);

    // If we don't have a cached Contact, try to find one with the given email.
    if (!$contacts) {
      $query = \Drupal::entityQuery('redhen_contact');
      $query->condition('email', $email, '=');
      $query->condition('status', $status);
      $results = $query->execute();

      // If we find any Contacts with emails that match our request,
      // load and return them.
      if (!empty($results)) {
        $contacts = Contact::loadMultiple(array_keys($results));
      }
    }

    return $contacts;
  }

  /**
   * {@inheritdoc}
   */
  public static function baseFieldDefinitions(EntityTypeInterface $entity_type) {
    $config = \Drupal::config('redhen_contact.settings');
    $fields = parent::baseFieldDefinitions($entity_type);

    $required_names = $config->get('required_properties');
    $fields['first_name'] = BaseFieldDefinition::create('string')
      ->setLabel(t('First Name'))
      ->setSettings([
        'max_length' => 50,
        'text_processing' => 0,
      ])
      ->setDefaultValue('')
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
        'weight' => -10,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE)
      ->setRequired($required_names['first_name'])
      ->setRevisionable(TRUE);

    $fields['middle_name'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Middle Name'))
      ->setSettings([
        'max_length' => 50,
        'text_processing' => 0,
      ])
      ->setDefaultValue('')
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
        'weight' => -9,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE)
      ->setRequired($required_names['middle_name'])
      ->setRevisionable(TRUE);

    $fields['last_name'] = BaseFieldDefinition::create('string')
      ->setLabel(t('Last Name'))
      ->setSettings([
        'max_length' => 50,
        'text_processing' => 0,
      ])
      ->setDefaultValue('')
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
        'weight' => -8,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE)
      ->setRequired($required_names['last_name'])
      ->setRevisionable(TRUE);

    $fields['email'] = BaseFieldDefinition::create('email')
      ->setLabel(t('Email'))
      ->setDefaultValue('')
      ->addConstraint('ContactEmailUnique')
      ->setDisplayOptions('view', [
        'label' => 'inline',
        'type' => 'email_mailto',
        'weight' => -4,
      ])
      ->setDisplayOptions('form', [
        'type' => 'string_textfield',
        'weight' => -4,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE)
      ->setRevisionable(TRUE)
      ->setRequired($config->get('valid_email'));

    $fields['uid'] = BaseFieldDefinition::create('entity_reference')
      ->setLabel(t('Linked user'))
      ->setDescription(t('The Drupal user this contact is linked to.'))
      ->setRevisionable(TRUE)
      ->setSetting('target_type', 'user')
      ->setTranslatable(FALSE)
      ->setRequired(FALSE)
      ->setDisplayOptions('view', [
        'label' => 'inline',
        'type' => 'entity_reference_label',
        'settings' => [
          'link' => TRUE,
        ],
        'weight' => 0,
      ])
      ->setDisplayOptions('form', [
        'type' => 'entity_reference_autocomplete',
        'weight' => 5,
        'settings' => [
          'match_operator' => 'CONTAINS',
          'size' => '60',
          'autocomplete_type' => 'tags',
          'placeholder' => '',
        ],
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setDisplayConfigurable('view', TRUE);

    $fields['status'] = BaseFieldDefinition::create('boolean')
      ->setLabel(t('Active'))
      ->setDescription(t('A boolean indicating whether the Contact is active.'))
      ->setDefaultValue(TRUE)
      ->setDisplayOptions('form', [
        'type' => 'boolean_checkbox',
        'settings' => [
          'display_label' => TRUE,
        ],
        'weight' => 16,
      ])
      ->setDisplayConfigurable('form', TRUE)
      ->setRevisionable(TRUE);

    $fields['created'] = BaseFieldDefinition::create('created')
      ->setLabel(t('Created'))
      ->setDescription(t('The time that the contact was created.'))
      ->setRevisionable(TRUE);

    $fields['changed'] = BaseFieldDefinition::create('changed')
      ->setLabel(t('Changed'))
      ->setDescription(t('The time that the contact was last edited.'))
      ->setRevisionable(TRUE);

    return $fields;
  }

}
